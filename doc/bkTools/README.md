#bkTools
Collection of various C++17 tools

####BitVector
BitVector class. Dynamic (runtime) or static (constexpr, compile-time) size.

####Clock
A simple stopwatch

####Color
Color, ColorBar, TransferFunction

####GraphCut
This n-dimensional graph cut implements the following two papers:

- Yuri Boykov and Vladimir Kolmogorov:
"An Experimental Comparison of Min-Cut/Max-Flow Algorithms for Energy Minimization in Vision"
IEEE Transactions on Pattern Analysis and Machine Intelligence 26(9):1124-37, 2004
    - class GraphCut\<nDims> 
    
- Jiangyu Liu and Jian Sun:
"Parallel Graph-cuts by Adaptive Bottom-up Merging"
Proceedings of IEEE Conference on Computer Vision and Pattern Recognition, pp. 2181-8, 2010
    - class GraphCutParallel\<nDims>

Remarks:
- works with custom image classes (regular grids)
    - requires access function as lambda (see example below)
- automatic usage of gradient-based edge weights with an exponential function
    - parameter "tolerance" for fine-tuning
- uses no diagonal neighborhood, i.e., 4 neighbors in 2D, 6 neighbors in 3D, etc.
- terminal weights have infinite capacity

####IO
File input/output helpers

####KDTree
N-dimensional KDTree implementation with templated point type. 
Point type must provide index operator[] to access elements.

####NDArray
Fixed size, n-dimensional, constexpr-ready array class

####NDVector
Dynamic size, n-dimensional vector class

####NDContainer
Convenience typedef. Chooses NDArray if template sizes are given, NDVector otherwise

####Progress
Keep track of algorithms' current progress.
Signals are used to communicate the current state.

####Signal
 A simple signal implementation, which is based on the one by Simon Schneegans, published in 2015
 and released into the public domain.
 
####Singleton
A simple singleton implementation

####string_utils
A collection of string manipulation utility functions

####ThreadPool
A simple threadpool