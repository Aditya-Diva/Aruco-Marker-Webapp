import cv2
import base64
from ezpysocket import ezpysocket as ps

from flask import Flask, render_template
from flask_socketio import SocketIO, emit

# Added for graphs
from flask import Response
import json
import random
import time
from datetime import datetime

from threading import Thread, Lock

# EzPyClient Generator Thread
g_thread = Thread()
# Flask init
app = Flask(__name__)
app.debug = True
socketio = SocketIO(app)

# TODO: Include flag to check if thread has stalled or not.

class Client:
    """[summary] Client class
    """
    # Static variables

    c = None  # EzPySocket Client
    dashboard_count = 0  # Confirms if all dashboard clients have been closed
    coords = []  # Placeholder for coords data from aruco marker detection
    stream_state = True  # False for pause, True for play
    client_crashed = False # Flag to monitor Client status

    # General Methods

    def start_client(self):
        """Starts the client on a thread
        """
        global g_thread
        # Start the EzPyClient thread only if the thread has not been started before.
        if not g_thread.isAlive():
            self.client_crashed = False
            print("Starting Thread for EzPyClient")
            g_thread = socketio.start_background_task(self.run_tcp_client)

    def stream_vid(self, image):
        """Responsible for updating the webcam feed on dashboard

        Args:
            image (cv2.Mat): Image that the dashboard will be updated with
        """
        # This part encodes numpy arrays into the format needed
        # Encode frame as jpeg
        # print(image.shape)
        frame = cv2.imencode('.jpg', image)[1].tobytes()
        # Encode frame in base64 representation and remove
        # utf-8 encoding
        frame = base64.b64encode(frame).decode('utf-8')
        # return "data:image/jpeg;base64,{}".format(frame)
        socketio.emit('update_cam_frame', {
            "image": "data:image/jpeg;base64,{}".format(frame)}, namespace='/')
        socketio.sleep(0.000001)

    # Application specific methods

    def run_tcp_client(self):
        """Sets up and runs the actual tcp client
        """
        print("Initializing camera...")
        cam = cv2.VideoCapture(0)
        cam.set(cv2.CAP_PROP_BUFFERSIZE, 1)
        print("Press key 'q' to exit !")
        if self.c is None:
            self.c = ps.EzPySocket(tokens=["start", "end"],
                                   reconnect_on_address_busy=5.0,
                                   server_mode=False)
            # Set configuration as needed (Increase this value incase end token is
            # not being read, i.e. give time for packets to be written properly from
            # server end
            self.c.set_sleep_between_packets(0.00005)

        data = {"camera": cam}

        try:
            self.c.client_loop(self.client_operation, data)
        except:
            print("Client seems to have crashed unexpectedly...")
            self.client_crashed = True

        cv2.destroyAllWindows()
        cam.release()

    def client_operation(self, c: ps.EzPySocket, data: dict):
        """The actual function that will be looped while streaming

        Args:
            c (ps.EzPySocket): Socket object
            data (dict): Collection of all data needs to be passed through, this
            can be a empty dict too
        """
        # Collect data
        ret, frame = data["camera"].read()
        if ret:

            # Send image to server
            c.send_image(frame)

            # Receive image from server
            frame = c.receive_image()

            self.coords = c.receive_float_list()

            # cv2.imshow("frame", frame)
            self.stream_vid(frame)
            if cv2.waitKey(1) == ord('q') or not self.stream_state:
                c.stop_loop()
        else:
            print("Not getting any frames...")


# Create a global client object
g_client = Client()


@app.route("/")
def home():
    """Default home page rendering

    Returns:
        string: loads home page with state_button content
    """
    global g_client
    print(g_client.stream_state)
    stream_state_button_content = "Pause" if g_client.stream_state == True else "Play"
    return render_template("home.html", stream_state_button_content=stream_state_button_content)


@socketio.on('connect')
def connected(resume_flag: bool = False):
    """Handling of dashboards being opened, or play button

    Args:
        resume_flag (bool, optional): If the connect is being called from pressing the Play button
        instead of a new dashboard being opened. Defaults to False.
    """
    print("\nConnected with webapp...\n")
    global g_thread, g_client

    if not resume_flag:
        g_client.dashboard_count += 1

    print("Current dashboard count : ", g_client.dashboard_count)

    # When all dashboards are killed and a new one is accessed, start in play state by default
    if g_client.dashboard_count == 1:
        g_client.stream_state = True

    try:
        # Check if this is the first init (where thread will be Thread())
        if not g_thread.isAlive():
            print("Thread is not alive.. Starting for the first time")
            g_client.start_client()
    except:  # If client was already initialized before as socketio.start_background_task() , does not have isAlive() attribute
        if g_thread.successful() and g_client.stream_state:  # Client was closed successfully from before, start anew
            print(
                "Previous thread was closed successfully, now starting another thread...")
            g_thread = Thread()
            g_client.start_client()
        else: # A previous thread is still running, use the same
            if g_client.client_crashed:
                print("Previous thread seems to have crashed, starting a new one...")
                g_thread = Thread()
                g_client.start_client()
            else:
                print("Previous thread has not closed yet, polling from the same...")


@socketio.on('disconnect')
def disconnected(pause_flag: bool = False):
    """Handling of dashboards being closed, or pause button

    Args:
        pause_flag (bool, optional): If the disconnect is being called from pressing the Pause button
        instead of the dashboard being closed. Defaults to False.
    """
    print("\nDisconnected with webapp...\n")
    global g_client
    if not pause_flag:
        g_client.dashboard_count -= 1
    g_client.dashboard_count = 0 if g_client.dashboard_count < 0 else g_client.dashboard_count
    print("Updated dashboard count", g_client.dashboard_count)
    # If no more dashboards are open OR application is on pause, close client instance
    if not g_client.dashboard_count or pause_flag:
        g_client.stream_state = False

        # Wait for thread operation to finish completely
        print("Waiting for thread to join...")
        g_thread.join()
        print("Thread has joined...")

        # Kill the client properly
        try:
            g_client.c.disconnect()
        except:
            pass
        g_client.c = None


@socketio.on('my_message')
def debug_message(msg):
    print("\n" + msg["data"] + "\n")


@socketio.on('toggle_stream_state')
def toggle_stream_state():
    """Callback to handle the stream state button being pressed.
    """
    global g_thread, g_client
    print("Previous Play state", g_client.stream_state)
    g_client.stream_state = not g_client.stream_state
    print("Toggling stream state")
    print("Updated Play state : ", g_client.stream_state)
    if g_client.stream_state:
        # Start the EzPyClient thread only if the thread has not been started before.
        connected(resume_flag=True)
    if not g_client.stream_state:
        disconnected(pause_flag=True)

    stream_state_button_content = "Pause" if g_client.stream_state == True else "Play"
    socketio.emit('update_stream_state_button_content', {
                  "content": stream_state_button_content})


@app.route('/chart-data')
def chart_data():
    """Generate chart data
    """
    def generate_random_data():
        global g_client
        while g_client.stream_state:
            if len(g_client.coords) == 0:
                g_client.coords = [None, None, None]
            json_data = json.dumps(
                {'time': datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
                 'valuex': g_client.coords[0],
                 'valuey': g_client.coords[1],
                 'valuez': g_client.coords[2]})
            print(json_data)
            yield f"data:{json_data}\n\n"
            time.sleep(1)

    return Response(generate_random_data(), mimetype='text/event-stream')


if __name__ == "__main__":
    socketio.run(app=app)
