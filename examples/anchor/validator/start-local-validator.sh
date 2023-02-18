#!/bin/bash

solana-test-validator --reset \
  --bpf-program DcGPyWLy2f7ScEK5SuqNQbkxboAQ7crBbnBmf3ZNbzqv  ./basic_0.so \
  --bpf-program 2f7rR9YhMhwadX5huTuWLjRKUXiLyGxsPDUP36Uxvtyr  ./basic_1.so \
  --bpf-program uDgNTSMh9owwbjLZNtRHzpKiZMu7KMCRwJHDd4ikRbm   ./basic_2.so \
  --bpf-program HmbTLCmaGvZhKnn1Zfa1JVnp7vkMV4DYVxPLWBVoN65L  ./basic_3_puppet_master.so \
  --bpf-program Fg6PaFpoGXkYsidMpWTK6W2BeZ7FEfcYkg476zPFsLnS  ./basic_3_puppet.so \
  --bpf-program CwrqeMj2U8tFr1Rhkgwc84tpAsqbt9pTt2a4taoTADPr  ./basic_4.so
