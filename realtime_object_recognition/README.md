**command to run and build the executable:**

`g++ -v main_rec.cpp -o main_obj -I /usr/local/include -L /usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -lopencv_calib3d`

**press** `x`: then the program computes threholded image of object in frame, cleanup of the threshold image using dilation and erosion, computes segmentation based on colors, computes the bounding box and axis of least central moment around the biggest

**press** `t`: the program goes into training mode and all the computation for the specific image takes place and the user is prompted to enter the object name and the frame is saved in 'database.csv'
