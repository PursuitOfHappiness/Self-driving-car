Place folder in automotive/miniature

The prototype is a TimeTriggeredConferenceClientModule that changes the distance measurement of one sensor in sensorboardata.
Check ViewLiveData.cpp in src folder for details on implementation.

To run it, follow the instructions in boxparker as they are similar.

Build ViewLiveData from sources.

Start "odsupercomponent" for the software component lifecycle management (this step will download the Docker image):

    $ docker run --rm=true --net host -v /home/$USER/config:/opt/data -w "/opt/data" -t -i seresearch/odsimulation:latest odsupercomponent --cid=111 --verbose=1

Next, open a new terminal tab and start the vehicle dynamics simulation "vehicle":

    $ docker run --rm=true --net host -v /home/$USER/config:/opt/data -w "/opt/data" -t -i seresearch/odsimulation:latest odsimvehicle --cid=111 --freq=10

Open a new terminal tab and start the visualization environment "cockpit" (the first command grants access to your Xserver):

    $ xhost +

    $ docker run --rm=true --net host -w "/opt/data" -t -i -e DISPLAY=$DISPLAY -e QT_X11_NO_MITSHM=1 -v /home/$USER/config:/opt/data -v /tmp/.X11-unix:/tmp/.X11-unix seresearch/odsimulation:latest odcockpit --cid=111


Do not run odsimirus as it will override distances with the ones calculated by it.

Once you save a recording (csv file), it will be stored within the container. 
To get the file to host you have to:
docker ps lists containers. Look for container ID that is running odcockpit

docker commit will make a snapshot of the container (optional, can use container directly)
docker rmi deletes a snapshot

docker run -t -i "containerID" /bin/bash will connect to the container(or snapshot).
docker cp containerID:locationOfFile destinationHost will copy the file at the container path to host path


