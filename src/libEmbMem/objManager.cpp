#ifndef __OBJ_MANAGER_CPP__
#define __OBJ_MANAGER_CPP__

#define BYTE        8
#define BUFFER_DIM  BYTE * sizeof(int)  

/* language extensions */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

/* Including the Class header */
#include "libEmbMem/objManager.h"

template<class T>
ObjManager<T>::ObjManager( unsigned int reqObj , ObjManager<T>* father ) :
                                        prev(father), next(NULL), firstFreeMask(0) {

  /* Here we decide how many blocks to allocate */
  if( 0 >= numOfBlocks = ( unsigned int ) ceil( ( ( float ) reqObj ) / BUFFER_DIM ) )
    numOfBlocks = 1 ;

  /* Internal buffer allocation */
  objArray = ( T* ) malloc( numOfBlocks * BUFFER_DIM * sizeof( T ) ) ;
  if( ! objArray ) {
    do {
      // printf( "ERROR ON BUFFER CREATION (%d blocks) : RESIZING !!!\n" , numOfBlocks ) ;
      numOfBlocks = ( unsigned int ) ceil( ( ( float ) numOfBlocks ) / 2 ) ;
      if( numOfBlocks )
        objArray = ( T* ) malloc( numOfBlocks * BUFFER_DIM * sizeof( T ) ) ;
    }
    while( ! objArray ) ;
  }

  //Initializing the UsageMask Pointer
  usageMask = new int[ numOfBlocks ] ;
  memset( usageMask , 0x0 , numOfBlocks * sizeof(int) ) ;

  if( ! father ) {
	nameBuffer      = new CharBuffer( DEFAULT_NAME_LEN * numOfBlocks * BUFFER_DIM ) ;
 	valueBuffer     = new CharBuffer( DEFAULT_NAME_LEN * numOfBlocks * BUFFER_DIM ) ;
  }
  else {
    nameBuffer      = father->nameBuffer ;
    valueBuffer     = father->valueBuffer ;
  }

}


template<class T>
ObjManager<T>::~ObjManager() {
  if( ! prev )
    delete nameBuffer , delete valueBuffer ;
  delete next ;
  if( prev )
    prev->next = NULL ;
  delete [] usageMask ;
  free( (void *) objArray ) ;
}


template<class T>
T* ObjManager<T>::getFreeObj( ObjManager<T>* &effectiveManager ) {
  unsigned int arrayCount , bitCount , output ;
  arrayCount = output = firstFreeMask ;
  while( arrayCount < numOfBlocks ) {
    bitCount = 0 ;
    while( bitCount < BUFFER_DIM ) {
      output = ( usageMask[ arrayCount ] >> ( BUFFER_DIM - 1 - bitCount ) ) & 0x1 ;
      if( ! output ) {
        usageMask[ arrayCount ] = usageMask[ arrayCount ] | ( 0x1 << ( BUFFER_DIM - 1 - bitCount ) ) ;
        effectiveManager = this ;
        firstFreeMask = arrayCount ;
        return ( objArray + ( BUFFER_DIM * arrayCount ) + bitCount ) ;
      }
      ++ bitCount ;
    }
    ++ arrayCount ;
  }
  if( ! next )
    next = new ObjManager<T>( numOfBlocks * BUFFER_DIM , this ) ;
  // Recoursive!
  return next -> getFreeObj( effectiveManager ) ;
}


template<class T>
void ObjManager<T>::freeObj( const T* obj ) {
  int displacement =  ( obj - objArray ) ;
  if( ( displacement >= ( numOfBlocks * BUFFER_DIM ) ) || displacement < 0 )
    if ( next )
      return next->freeObj( obj ) ; //EXCEPTION ???

  unsigned int arrayNum = ( unsigned int ) floor( displacement / BUFFER_DIM ) ;
  int bitNum = displacement % BUFFER_DIM ;
  usageMask[ arrayNum ] = usageMask[ arrayNum ] & ( ~ ( 0x1 << ( BUFFER_DIM - 1 - bitNum ) ) ) ;

  if ( firstFreeMask > arrayNum )
    firstFreeMask = arrayNum ;
}

template<class T>
void ObjManager<T>::clean() {
  nameBuffer->clean() ;
  valueBuffer->clean() ;
  for( unsigned int i = 0 ; i<numOfBlocks ; ++i ) {
    usageMask[i] = 0 ;
  }
  if( next )
    next->clean() ;
}

#endif

