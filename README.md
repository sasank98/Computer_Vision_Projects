# Computer_Vision_Implementations

 This repository contains my implementation of different Classical computer vision and Deep learning algorithms

**sift_detection:** This project contains my implementation of the SIFT detector. I implemented Harris corner to detect the edges and Keypoints and SIFT to describe each keypoint.

![Notredame SIFT detections](sift_detection\results\notredame.png)

**fully_connected_network for image classification:** In this project I used NumPy to build forward and backward passes for fully connected layers and activation functions. I used the built layers to build an image classifier but couldn't get good performance because I wasn't able to initialize the weights in Xavier distribution.

![Training loss saturated after 10 epochs](fully_connected_network\results\training_loss.png)

**convolutional_network for image classification:** In this project I built Convolution layers and batch normalization layers to improve the networks performance compared to fully connected layers. Ended up getting similar results using convolution layers.

![Training seems overfitted](convolutional_network\results\training_loss.png)

Below is a plot of filters from the intermediate layers, they show that the model is learning meaningful features

![Filters from intermediate layers](convolutional_network\results\filters.png)

**visual_odometry:** In this project I implemented Visual Odometry on Kitti Dataset. I used SIFT to detect and compute features, and matched them. I used the matches to compute the essential matrix and pose between frames from it.

Below is a plot of Ground truth and generated trajectory
Red dots denote ground-truth camera poses and green ones are the estimations

![Trajectory and ground truth](visual_odometry\results\trajectory_and_gt.png)
