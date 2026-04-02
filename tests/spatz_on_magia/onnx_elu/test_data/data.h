/* Automatically generated header file for Spatz ONNX testing */
#ifndef DATA_H_
#define DATA_H_

#define LEN 16

static const float16 alpha = 1.000000f;

#if 0
static const float16 input_vec[] = { -0.570801f, 0.299316f, 0.697266f, -1.122070f, 0.062805f, -1.653320f, 0.252441f, 0.715332f, 0.229370f, 1.640625f, -0.834961f, 0.383789f, -0.982910f, 1.237305f, 0.419922f, -0.902832f };
static const float16 expected_vec[] = { -0.434814f, 0.299316f, 0.697266f, -0.674316f, 0.062805f, -0.808594f, 0.252441f, 0.715332f, 0.229370f, 1.640625f, -0.565918f, 0.383789f, -0.625977f, 1.237305f, 0.419922f, -0.594727f };
#endif

#if 1
/* alternated */
static const float16 input_vec[] = { 1.5f, -1.5f, 1.5f, -1.5f, 1.5f, -1.5f, 1.5f, -1.5f, 1.5f, -1.5f, 1.5f, -1.5f, 1.5f, -1.5f, 1.5f, -1.5f };
static const float16 expected_vec[] = { 1.5f, -0.776855f, 1.5f, -0.776855f, 1.5f, -0.776855f, 1.5f, -0.776855f, 1.5f, -0.776855f, 1.5f, -0.776855f, 1.5f, -0.776855f, 1.5f, -0.776855f };
#endif

#if 0
/* all negatives */
static const float16 input_vec[] = { -1.5f, -1.5f, -1.5f, -1.5f, -1.5f, -1.5f, -1.5f, -1.5f, -1.5f, -1.5f, -1.5f, -1.5f, -1.5f, -1.5f, -1.5f, -1.5f };
static const float16 expected_vec[] = { -0.776855f, -0.776855f, -0.776855f, -0.776855f, -0.776855f, -0.776855f, -0.776855f, -0.776855f, -0.776855f, -0.776855f, -0.776855f, -0.776855f, -0.776855f, -0.776855f, -0.776855f, -0.776855f };
#endif

#if 0
/* all positives */
static const float16 input_vec[] = { 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f };
static const float16 expected_vec[] = { 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f };
#endif

#endif   /* DATA_H_ */
