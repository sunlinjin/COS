/*
 o---------------------------------------------------------------------o
 |
 | COS Array - basic arrays
 |
 o---------------------------------------------------------------------o
 |
 | C Object System
 |
 | Copyright (c) 2006+ Laurent Deniau, laurent.deniau@cern.ch
 |
 | For more information, see:
 | http://cern.ch/laurent.deniau/cos.html
 |
 o---------------------------------------------------------------------o
 |
 | This file is part of the C Object System framework.
 |
 | The C Object System is free software; you can redistribute it and/or
 | modify it under the terms of the GNU Lesser General Public License
 | as published by the Free Software Foundation; either version 3 of
 | the License, or (at your option) any later version.
 |
 | The C Object System is distributed in the hope that it will be
 | useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 | of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 |
 | See <http://www.gnu.org/licenses> for more details.
 |
 o---------------------------------------------------------------------o
 |
 | $Id: Array.c,v 1.36 2009/08/15 14:54:58 ldeniau Exp $
 |
*/

#include <cos/Array.h>
#include <cos/Functor.h>
#include <cos/IntVector.h>
#include <cos/Number.h>

#include <cos/gen/container.h>
#include <cos/gen/functor.h>
#include <cos/gen/init.h>
#include <cos/gen/new.h>
#include <cos/gen/object.h>
#include <cos/gen/sequence.h>
#include <cos/gen/value.h>

#include <cos/prp/object.h>
#include <cos/prp/sequence.h>

#include <stdlib.h>

// -----

makclass(Array, Sequence);

makclass(Array0, Array);
makclass(Array1, Array);
makclass(Array2, Array);
makclass(Array3, Array);
makclass(Array4, Array);
makclass(Array5, Array);
makclass(Array6, Array);
makclass(Array7, Array);
makclass(Array8, Array);
makclass(Array9, Array);
makclass(ArrayN, Array);

// -----

useclass(ExBadAlloc);
useclass(Array, ArrayView);

// ----- properties

#define size_to_OBJ(siz) gautoDelete(aInt(siz))
#define array_class(arr) Array

defproperty(Array,   size , size_to_OBJ);
defproperty(Array, ()class, array_class);

#undef size_to_OBJ
#undef array_class

// --- getters

defmethod(struct Slice, gslice, Array)
  retmethod( *atSlice(0,self->size,self->stride) );
endmethod

defmethod(U32, gsize, Array)
  retmethod(self->size);
endmethod

defmethod(OBJ*, gobjPtr, Array)
  retmethod(self->object);
endmethod

defmethod(OBJ, gisEmpty, Array)
  retmethod(self->size ? False : True);
endmethod

defmethod(OBJ, gclass, Array)
  retmethod(Array);     // class cluster: hide implementation details
endmethod

// ----- allocator

struct Array*
Array_alloc(U32 size)
{
  enum { N = 10 };
  static struct Class* cls[] = {
    classref(Array0,Array1,Array2,Array3,Array4,
             Array5,Array6,Array7,Array8,Array9,
             ArrayN)
  };

  OBJ _cls = (OBJ)cls[size > N ? N : size];
  OBJ _arr = gallocWithSize(_cls, size * sizeof(OBJ));

  struct ArrayN *arrn = STATIC_CAST(struct ArrayN*, _arr);
  struct Array  *arr  = &arrn->Array;

  arr->object = arrn->_object;
  arr->size   = size;
  arr->stride = 1;

  return arr;
}

// ----- constructors

defmethod(OBJ, galloc, pmArray) // lazy alloc
  retmethod(_1);
endmethod

defalias (OBJ, (ginitWith)gnewWith, pmArray, Array);
defmethod(OBJ,  ginitWith         , pmArray, Array) // clone
  PRE
  POST
    // automatically trigger ginvariant

  BODY
    struct Array* arr = Array_alloc(self2->size);
    OBJ _arr = (OBJ)arr; PRT(_arr);

    OBJ *dst   = arr->object;
    OBJ *end   = arr->object + arr->size;
    OBJ *src   = self2->object;
    I32  src_s = self2->stride;

    while (dst != end) {
      *dst++ = gretain(*src);
      src += src_s;
    }

    UNPRT(_arr);
    retmethod(_arr);
endmethod

defalias (OBJ, (ginitWith)gnewWith, pmArray, Slice);
defmethod(OBJ,  ginitWith         , pmArray, Slice) // Int sequence
  struct Array* arr;

  PRE
  POST
    U32 size = Slice_size(self2);
    for (U32 i = 0; i < size; i++)
      test_assert( gint(arr->object[i]) == Slice_eval(self2,i) );

  BODY
    U32 size = Slice_size(self2);
    arr = Array_alloc(size);
    OBJ _arr = (OBJ)arr; PRT(_arr);
  
    for (U32 i = 0; i < size; i++)
      arr->object[i] = gretain(aInt(Slice_eval(self2,i)));

    UNPRT(_arr);
    retmethod(_arr);
endmethod

defalias (OBJ, (ginitWith)gnewWith, pmArray, Range);
defmethod(OBJ,  ginitWith         , pmArray, Range) // Int sequence
  struct Array* arr;

  PRE
  POST
    U32 size = Range_size(self2);
    for (U32 i = 0; i < size; i++)
      test_assert( gint(arr->object[i]) == Range_eval(self2,i) );

  BODY
    U32 size = Range_size(self2);
    arr = Array_alloc(size);
    OBJ _arr = (OBJ)arr; PRT(_arr);
  
    for (U32 i = 0; i < size; i++)
      arr->object[i] = gretain(aInt(Range_eval(self2,i)));

    UNPRT(_arr);
    retmethod(_arr);
endmethod

defalias (OBJ, (ginitWith2)gnewWith2, pmArray, Int, Object);
defmethod(OBJ,  ginitWith2          , pmArray, Int, Object) // element
  PRE
    test_assert(self2->value >= 0, "negative array size");

  POST
    // automatically trigger ginvariant

  BODY
    struct Array* arr = Array_alloc(self2->value);
    OBJ _arr = (OBJ)arr; PRT(_arr);

    OBJ *dst = arr->object;
    OBJ *end = arr->object + arr->size;

    while (dst != end)
      *dst++ = gretain(_3);

    UNPRT(_arr);
    retmethod(_arr);
endmethod

defalias (OBJ, (ginitWith2)gnewWith2, pmArray, Int, Functor);
defmethod(OBJ,  ginitWith2          , pmArray, Int, Functor) // generator
  PRE
    test_assert(self2->value >= 0, "negative array size");

  POST
    // automatically trigger ginvariant

  BODY
    struct Array* arr = Array_alloc(self2->value);
    OBJ _arr = (OBJ)arr; PRT(_arr);

    OBJ *dst = arr->object;
    OBJ *end = arr->object + arr->size;
    int argc = garity(_3);

    if (!argc)
      while (dst != end)
        *dst++ = gretain(geval(_3));

    else
      for (I32 i = 0; dst != end; i++)
        *dst++ = gretain(geval1(_3, aInt(i)));

    UNPRT(_arr);
    retmethod(_arr);
endmethod

defalias (OBJ, (ginitWith2)gnewWith2, pmArray, Array, Slice);
defmethod(OBJ,  ginitWith2          , pmArray, Array, Slice) // sub array
  PRE
    U32 first = Slice_first (self3);
    U32 last  = Slice_last  (self3);

    test_assert( first < self2->size &&
                 last  < self2->size, "slice out of range" );

  POST
    // automatically trigger ginvariant

  BODY
    U32 start  = Slice_first (self3)*self2->stride;
    I32 stride = Slice_stride(self3)*self2->stride;

    struct Array* arr = Array_alloc(self3->size);
    OBJ _arr = (OBJ)arr; PRT(_arr);

    OBJ *dst   = arr->object;
    OBJ *end   = arr->object + arr->size;
    OBJ *src   = self2->object + start;
    I32  src_s = stride;

    while (dst != end) {
      *dst++ = gretain(*src);
      src += src_s;
    }

    UNPRT(_arr);
    retmethod(_arr);
endmethod

defalias (OBJ, (ginitWith2)gnewWith2, pmArray, Array, IntVector);
defmethod(OBJ,  ginitWith2          , pmArray, Array, IntVector) // random sequence
  PRE
  POST
    // automatically trigger ginvariant

  BODY
    struct Array* arr = Array_alloc(self3->size);
    OBJ _arr = (OBJ)arr; PRT(_arr);

    OBJ *dst   = arr->object;
    OBJ *end   = arr->object + arr->size;
    OBJ *src   = self2->object;
    U32  src_z = self2->size;
    I32  src_s = self2->stride;
    I32 *idx   = self3->value;
    I32  idx_s = self3->stride;

    while (dst != end) {
      U32 i = Range_index(*idx, src_z);
      test_assert( i < src_z, "index out of range" );
      *dst++ = gretain(src[i*src_s]);
      idx += idx_s;
    }

    UNPRT(_arr);
    retmethod(_arr);
endmethod

// ----- destructor

defmethod(OBJ, gdeinit, Array)
  OBJ *obj = self->object;
  OBJ *end = self->object + self->size;

  while (obj != end && *obj)    // take care of protection cases
    grelease(*obj++);

  retmethod(_1);
endmethod

// ----- invariant

defmethod(void, ginvariant, Array, (STR)func, (STR)file, (int)line)
  OBJ *obj   = self->object;
  OBJ *end   = self->object + self->size*self->stride;
  I32  obj_s = self->stride;

  while (obj != end && *obj)
    obj += obj_s;

  test_assert( obj == end,
               "Array contains null elements", func, file, line);
endmethod

