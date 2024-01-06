**command to run and build the executable:**

`g++ -v main_vid.cpp -o main_vid -I /usr/local/include -L /usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -lopencv_calib3d`

**shown a prompt:** Save image or not?: `1 or 0` > The user must type either `1` for Yes or `0` for No and press enter.

These images are essentially frames where the chessboard corners are detected. 

If the user selects `1` in the previous step, another prompt is shown: "Choose Augmentation: `axes`, `cube`, `pillars`:" the user must type one of them

**press** 's': With chessboard in the frame when s is pressed the program extracts the chessboard corners. This process is repeated 5 times at least before the user is prompted: `Run Calibration on images: 1 or 0:` > The user must type either `1` for Yes or `0` for No and press enter.

**press** `t`: With chessboard in frame when 't' is pressed the program shows the image of the chessboard with the augmented 

**press** `h`: With pattern in frame when 'h' is pressed the program shows the harris corner extracted image of the pattern 

**press** `q`: The program quits