BEGIN {
  i = 0;
  printf("#include \"opcode.h\"\n\nconst Operation opcodes[256] = {\n");
} 
{
  j = strtonum("0x" $3);
  while(i < j) {
    printf("  {UND, A_UND, -1, -1},\n");
    i+=1;
  }
  printf("  {%s, %s, %d, %d},\n", $2, $1, $4, $5);
  i+=1;
}
END {
  printf("};\n");
}
