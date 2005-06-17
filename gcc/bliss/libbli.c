// 32bit version only

// argument numbers is a potential problem

// later: put some that can be ctce into parse.y

typedef unsigned int size_t;
#define NULL 0

static void *memchr(const void *s, int c, size_t n)
{
  const unsigned char *p = s;
  while (n-- != 0) {
    if ((unsigned char)c == *p++) {
      return (void *)(p-1);
    }
  }
  return NULL;
}

static void *mymemchr_not(const void *s, int c, size_t n)
{
  const unsigned char *p = s;
  while (n-- != 0) {
    if ((unsigned char)c != *p++) {
      return (void *)(p-1);
    }
  }
  return NULL;
}

static int memcmp(const void * cs,const void * ct,size_t count)
{
  const unsigned char *su1, *su2;
  signed char res = 0;

  for( su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
    if ((res = *su1 - *su2) != 0)
      break;
  return res;
}

static void * memcpy(void * dest,const void *src,size_t count)
{
  char *tmp = (char *) dest, *s = (char *) src;

  while (count--)
    *tmp++ = *s++;

  return dest;
}

static void * memset(void * s,int c,size_t count)
{
  char *xs = (char *) s;

  while (count--)
    *xs++ = c;

  return s;
}

// maybe ctce
long ch$allocation(int n, int cs) {
  if (cs==0)
    cs=8;
  return 1+((n*cs)>>5);
}

// maybe ctce
// null term?
long ch$size(char * ptr) {
  int ret=8;
  return ret; // check. think so
  if (ptr==0)
    return ret;
  for (; *ptr; ptr++,ret+=8) ;
  return ret;
}

long ch$ptr(char * addr, int i, int cs) {
  if (cs==0)
    cs=8;
  return addr+i;
}

// maybe ltce
long ch$plus(char * ptr, int i) {
  return ptr+i;
}

long ch$diff(char * ptr1, char * ptr2) {
  return ptr1-ptr2;
}

long ch$rchar(char * ptr) {
  return *ptr;
}

long ch$a_rchar(char ** addr) {
  char * ptr = *addr;
  ptr++;
  long ret = *ptr;
  *addr = ptr;
  return ret;
}

long ch$rchar_a(char ** addr) {
  char * ptr = *addr;
  long ret = *ptr;
  ptr++;
  *addr = ptr;
  return ret;
}

void ch$wchar(char c, char * ptr) {
  *ptr = c;
}

void ch$a_wchar(char c, char ** addr) {
  char * ptr = *addr;
  ptr++;
  *ptr = c;
  *addr = ptr;
}

void ch$wchar_a(char c, char ** addr) {
  char * ptr = *addr;
  *ptr = c;
  ptr++;
  *addr = ptr;
}

long ch$move(int n, char * sptr, char * dptr) {
  memcpy(dptr, sptr, n);
  return dptr+n;
}

ch$fill(char fill, int dn, char * dptr) {
  memset(dptr, fill, dn);
  return dptr+dn;
}

// not yet: ch$copy as something with vararg

static enum { eql, neq, lss, leq, gtr, geq, compare };

static signed int mymemcmp(const void * cs,const void * ct,size_t counts, size_t countt, char fill, int type)
{
  size_t count = ( counts < countt ? counts : countt);
  size_t count2 = ( counts > countt ? counts : countt);
  count2-=count;
  const unsigned char *su1, *su2;
  signed char res = 0;

  for( su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
    if ((res = *su1 - *su2) != 0)
      break;
  count=count2;
  if (count) 
    for( ; 0 < count; ++su1, ++su2, count--) {
      unsigned char c1, c2;
      if (counts < countt) {
	c1 = fill;
	c2 = *su2;
      } else {
	c1 = *su1;
	c2 = fill;
      }
      if ((res = c1 - c2) != 0)
	break;
    }
  if (type==eql && res==0)
    return 1;
  if (type==neq && res!=0)
    return 1;
  if (type==lss && res<0)
    return 1;
  if (type==leq && res<=0)
    return 1;
  if (type==gtr && res>0)
    return 1;
  if (type==geq && res>=0)
    return 1;
  if (type==compare && res<0)
    return -1;
  if (type==compare && res>0)
    return -1;
  return 0;
}

long ch$eql(int n1, char * ptr1, int n2, char *ptr2, char fill) {
  return mymemcmp(ptr1,ptr2,n1,n2,fill,eql);
}

long ch$neq(int n1, char * ptr1, int n2, char *ptr2, char fill) {
  return mymemcmp(ptr1,ptr2,n1,n2,fill,neq);
}

long ch$lss(int n1, char * ptr1, int n2, char *ptr2, char fill) {
  return mymemcmp(ptr1,ptr2,n1,n2,fill,lss);
}

long ch$leq(int n1, char * ptr1, int n2, char *ptr2, char fill) {
  return mymemcmp(ptr1,ptr2,n1,n2,fill,leq);
}

long ch$gtr(int n1, char * ptr1, int n2, char *ptr2, char fill) {
  return mymemcmp(ptr1,ptr2,n1,n2,fill,gtr);
}

long ch$geq(int n1, char * ptr1, int n2, char *ptr2, char fill) {
  return mymemcmp(ptr1,ptr2,n1,n2,fill,geq);
}

long ch$compare(int n1, char * ptr1, int n2, char *ptr2, char fill) {
  return mymemcmp(ptr1,ptr2,n1,n2,fill,compare);
}

long ch$find_ch(int n, char * ptr, char char_) {
  return memchr(ptr, char_, n);
}

long ch$find_not_ch(int n, char * ptr, char char_) {
  return mymemchr_not(ptr, char_, n);
}

long ch$fail(char * ptr) {
  return !ptr;
}

static char * mymemmem(const char * s1,const char * s2, int n1, int n2)
{
  int l1, l2;

  l2 = n2;
  if (!l2)
    return (char *) s1;
  l1 = n1;
  while (l1 >= l2) {
    l1--;
    if (!memcmp(s1,s2,l2))
      return (char *) s1;
    s1++;
  }
  return NULL;
}

long ch$find_sub(int cn, char * cptr, int pn, char * pptr) {
  return mymemmem(cptr, pptr, cn, pn);
}

// ch$transtable ch$translate not yet


