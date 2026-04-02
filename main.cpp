#include <iostream>
#include <vector>
using namespace std;

class Tensor {

    public:
    Tensor ( const  vector < size_t >& shape , const   vector <double >& values ) ;
};


int main(){


    Tensor t({3.4},{1,2,3,4,5,6,6,7,7,7,7,12})
    return 0;

}