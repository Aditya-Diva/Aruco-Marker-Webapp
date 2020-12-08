echo "Choose your preference:"
echo "1. Run demo directly"
echo "2. Run docker in dev/play mode"
read -p "Choose how you would like to launch the docker: " choice

CMD_SERVER=".\/compile_and_run.sh"
CMD_CLIENT="python3\",\"run_client.py"

if [ $choice -eq 1 ] 
then
    echo -e '$d\nw\nq'| ed Dockerfile.server
    echo "CMD [\"$CMD_SERVER\"]" >> Dockerfile.server

    echo -e '$d\nw\nq'| ed Dockerfile.client
    echo "CMD [\"$CMD_CLIENT\"]" >> Dockerfile.client
elif [ $choice -eq 2 ]
then
    echo -e '$d\nw\nq'| ed Dockerfile.server
    echo "# CMD [\"$CMD_SERVER\"]" >> Dockerfile.server

    echo -e '$d\nw\nq'| ed Dockerfile.client
    echo "# CMD [\"$CMD_CLIENT\"]" >> Dockerfile.client
else
    echo "Unrecognized input. Please run again."
fi

docker build -f Dockerfile.server -t aruco-webapp:server .
docker build -f Dockerfile.client -t aruco-webapp:client .