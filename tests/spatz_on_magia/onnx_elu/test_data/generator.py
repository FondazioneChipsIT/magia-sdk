import argparse
import onnx
import os
import sys

import numpy as np
import onnxruntime as ort

def nonnegative_flaot(value):
    try:
        fvalue = float(value)
    except ValueError:
        raise argparse.ArgumentTypeError(f"'{value}' is not a valid real number.")

    if fvalue < 0:
        raise argparse.ArgumentTypeError(f"Alpha must be positive ({value}).")

    return fvalue

def parse_args():
    parser = argparse.ArgumentParser(description="Generator of Input Data and Golden Model for ONNX ELU test")

    parser.add_argument("length", type=int, help="Input vector length")
    parser.add_argument("alpha", type=nonnegative_flaot, help="Alpha value for ONNX ELU")

    args = parser.parse_args()
    return args.length, args.alpha

def generate_input_data(length):
    input = (np.random.randn(length)).astype(np.float16)
    return input

def run_onnx_elu(input, alpha):
    input_info = onnx.helper.make_tensor_value_info('I', onnx.TensorProto.FLOAT16, input.shape)
    output_info = onnx.helper.make_tensor_value_info('O', onnx.TensorProto.FLOAT16, input.shape)

    opset = onnx.helper.make_operatorsetid("", 22)
    node_def = onnx.helper.make_node('Elu', ['I'], ['O'], alpha=alpha)
    graph_def = onnx.helper.make_graph([node_def], 'onnx-elu-test', [input_info], [output_info])
    model_def = onnx.helper.make_model(graph_def, producer_name='onnx-generator', opset_imports=[opset])

    ses = ort.InferenceSession(model_def.SerializeToString())
    res = ses.run(None, {'I':input})

    return res[0]

def format_array(array):
    return "{ " + ", ".join(f"{x:f}f" for x in array) + " }"

def format_float(value):
    return f"{value:f}f"

def generate_header_file(length, alpha, input, expected, filename="data.h"):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    filepath = os.path.join(script_dir, filename)

    with open(filepath, "w") as f:
        f.write(f"/* Automatically generated header file for Spatz ONNX testing */\n")
        f.write(f"#ifndef DATA_H_\n")
        f.write(f"#define DATA_H_\n\n")

        f.write(f"#define LEN {length}\n\n")

        f.write(f"static const float16 alpha = {format_float(alpha)};\n")
        f.write(f"static const float16 input_vec[] = {format_array(input)};\n")
        f.write(f"static const float16 expected_vec[] = {format_array(expected)};\n\n")

        f.write (f"#endif   /* DATA_H_ */\n")

def main():
    length, alpha = parse_args()

    input = generate_input_data(length)

    expected = run_onnx_elu(input, alpha)

    generate_header_file(length, alpha, input, expected)

    print(f"File 'data.h' successfully generated with {length} elements.")


if __name__ == "__main__":
    main()
