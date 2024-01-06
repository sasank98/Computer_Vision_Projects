**command to run and build the executable:** We will be building two executables in this project one for image and one for video

For Video

`g++ main_vid.cpp -o main_vid -I /usr/local/include -L /usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -lopencv_calib3d`

For Image

`g++ main_img.cpp -o main_img -I /usr/local/include -L /usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -lopencv_calib3d`

filter.h - contains all function definitions

filter.cpp - contains all functions bodies

main_video.cpp - displays a video window that displays live video and a image window that displays the output of live video frame with after filtering (the type of filtering depends on the key pressed).   

main_img.cpp - displays an image when the location is given and exits code upon pressing 'q'.

In the extension the implemenation of erosion and dilation filtering has been performed where the size of the kernel is 5*5. Press '1' to get dilated image and '2' to get eroded image while vidDisplay.cpp is running. 