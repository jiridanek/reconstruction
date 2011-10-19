#ifndef FROMARTICLE_H
#define FROMARTICLE_H

//const int WHITE = 255;
//const int BLACK = 0;

//#define RESERVABLE_PRIORITY_QUEUE

template <class T> void Reconstruction_by_dillatation_bucket(i3d::Image3d<T> & MARKER, i3d::Image3d<T> & MASK, i3d::Image3d<T> & reconstructed);
void timeAlg2();

/**

  Testy (obrázek GRAY8, 256x256)

    10: 0.28404688
    100: 0.281822065
    1000: 2.818387677
    10000: 28.168290442

opět si zatím ještě neporadí s nemezními hodnotami v markeru

 */

#endif // FROMARTICLE_H
