import argparse
import onnx
import os
import numpy as np
import onnxruntime as ort

def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument("length", type=int, help="Input vector length")

    args = parser.parse_args()
    return args.length


def generate_input_data(length):
    input = (np.random.randn(length)).astype(np.float16)

    return input


def format_array(array):
    return "{ " + ", ".join(f"{x:f}f" for x in array) + " }"

def generate_header_file(length, input, filename="data.h"):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    filepath = os.path.join(script_dir, filename)

    with open(filepath, "w") as f:
        f.write(f"/* Automatically generated header file for Spatz ONNX testing */\n")
        f.write(f"#ifndef DATA_H_\n")
        f.write(f"#define DATA_H_\n\n")

        f.write(f"#define LEN {length}\n\n")

        f.write(f"static const float16 input_vec[] = {format_array(input)};\n\n")

        f.write(f"#endif   /* DATA_H_ */\n")


def main():
    length = parse_args()

    input = generate_input_data(length)

    generate_header_file(length, input)

    print(f"File 'data.h' successfully generated with {length} elements.")


if __name__ == "__main__":
    main()
