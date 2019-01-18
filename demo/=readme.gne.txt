ml purge
ml Python/3.6.3
ml foss/2017a
ml nvidia/driver
ml CMake/3.11.3-foss-2017a
ml SWIG/3.0.8-foss-2017a-Python-3.6.3
ml CUDA/9.0.176
ml TensorFlow/1.8.0-foss-2017a-CUDA-9.0.176-Python-3.6.3

bash
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/gstore/home/albertgo/dev/openmm/libtensorflow-gpu/1.8/lib
. activate openMM2

