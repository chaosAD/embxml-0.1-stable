#ifndef __OBJ_MANAGER_H__
#define __OBJ_MANAGER_H__

#include "libEmbMem/charBuffer.h"

#define BYTE        8
#define BUFFER_DIM  BYTE * sizeof(int)  

/* language extensions */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

template<class T>
class ObjManager {
private:
public:
  ObjManager<T>* prev ;
  ObjManager<T>* next ;
  unsigned int numOfBlocks ;    //each block is a group of 32 objects
  int* usageMask ;

  unsigned int firstFreeMask ;

  CharBuffer* nameBuffer ;
  CharBuffer* valueBuffer ;

  T* objArray ;

  // CONSTRUCTOR
  ObjManager( unsigned int reqObj , ObjManager<T>* father=NULL ) :
                                        prev(father), next(NULL), firstFreeMask(0) {

    /* Here we decide how many blocks to allocate */
    numOfBlocks = ( unsigned int ) ceil( ( ( float ) reqObj ) / BUFFER_DIM );
    if( 0 >= numOfBlocks )
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


  // DESTRUCTOR
  ~ObjManager() {
    if( ! prev )
      delete nameBuffer , delete valueBuffer ;
    delete next ;
    if( prev )
      prev->next = NULL ;
    delete [] usageMask ;
    free( (void *) objArray ) ;
  }



  // This represents the effective Object Manager
  T* getFreeObj( ObjManager<T>* &effectiveManager ) {
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


  // Releases a memory location
  void freeObj( const T* obj ) {
    int displacement =  ( obj - objArray ) ;
    if( ( displacement >= (int)( numOfBlocks * BUFFER_DIM ) ) || displacement < 0 )
      if ( next )
        return next->freeObj( obj ) ; //EXCEPTION ???
   
    unsigned int arrayNum = ( unsigned int ) floor( displacement / BUFFER_DIM ) ;
    unsigned int bitNum = displacement % BUFFER_DIM ;
    usageMask[ arrayNum ] = usageMask[ arrayNum ] & ( ~ ( 0x1 << ( BUFFER_DIM - 1 - bitNum ) ) ) ;
   
    if ( firstFreeMask > arrayNum )
      firstFreeMask = arrayNum ;
  }

  
  // Empty the entire ObjectManager 
  void clean() {
    nameBuffer->clean() ;
    valueBuffer->clean() ;
    for( unsigned int i = 0 ; i<numOfBlocks ; ++i ) {
      usageMask[i] = 0 ;
    }
    if( next )
      next->clean() ;
  }


};


#endif

