int amalloc(){
return malloc(4);
}
undecl(int x, int y){
printf("undecl %x %x\n",x,y);
}
undecl2(void){
printf("undecl2\n");
}
int d;
main(){
int c;
int ee_=3;
int *ee=&ee_;
printf("ee %x %x %x\n",ee_,ee,*ee);
printf("%x %x\n",&c, &d);
s();
t();
dummy();
pppp(777,333);
}
