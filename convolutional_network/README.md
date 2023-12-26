# CS 5330 programming assignment 4: Convolutional Neural Networks

We will use the same environment as the one used in the programming assignment 3. You can re-use that one if you like. Or you can create a new environment using the following instructions.


# Setup
1. Install [Miniconda](https://conda.io/miniconda.html). It doesn't matter whether you use Python 2 or 3 because we will create our own environment that uses python3 anyways.
2. Create a conda environment using the appropriate command. On Windows, open the installed "Conda prompt" to run the command. On MacOS and Linux, you can just use a terminal window to run the command, Run: `conda env create -f cnn_env.yml`
3. This will create an environment named 'cnn'. Activate it using the Windows command, `activate cnn` in the Windows default Command Prompt or the MacOS / Linux command, `conda activate cnn` or `source activate cnn`
4. Run the notebook using `jupyter notebook ./pa4_code/convolution_network.ipynb` to check implementation and visualize results