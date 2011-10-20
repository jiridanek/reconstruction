/*
  Paralelně: v každém cyklu vždycky provést pixely s nejvyšší prioritou z fronty. Možná taky trochu iterativně - klidně některou hodnotu přepisovat vícekrát
  iterativní djikstrův algoritmus z článku, zkusit blokování (může být těžké) a rozbalování smyček (mělo by být easy)
  */

/*!
  \todo FIND OUT:   const int WHITE?
                    templating of this method
 */

#include <cstring>
#include <iostream>
#include <cstdio>
#include <cassert>

#include <i3d/image3d.h>
#include "Point.h"
#include "ReconstructionAlgorithms.h"
#include "FromArticle.h"
#include "DownhillReconstruction.h"

using namespace std;

int main() {
#ifdef DEBUG
    std::count << "DEBUG" << endl;
#endif
    //timeAlg1();
    //timeAlg2();
    timeAlg3();
}
