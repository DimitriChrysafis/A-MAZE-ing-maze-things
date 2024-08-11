# Running Metal via Python only, on Mac

Minimal Python-only script for Mac, running a "Hello World" Metal kernel. No *need* for Xcode, a Swift app, or PyTorch.

```
# Step 0: Clone the gist
git clone https://gist.github.com/f7bb0cdd26c018f40052f9944fc5c679.git

# Step 1: Install prerequisites
pip install pyobjc

# Step 2: Run the file
python MyMetalKernel.py
```

You're now done with the Metal example.

- For how to use the `Metal` library, find the associated objective-C functions and documentation in [Apple Metal documentation](https://developer.apple.com/documentation/metal/).
- Any function or class in the official Objective-C Metal library is somehow bound to an object in the Python `Metal` library, made available via `pyobjc`. See [pyobjc documentation](https://pyobjc.readthedocs.io/en/latest/api/module-objc.html) for additional help.

For reference, the above script was tested on Ventura 13.2.1 with Python 3.11.5 on pyobjc=10.0.