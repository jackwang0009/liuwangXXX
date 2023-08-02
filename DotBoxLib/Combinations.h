class COMBINATIONS
{
  unsigned int table[11][41];
public:
  COMBINATIONS(void);
  ~COMBINATIONS(void){};
  unsigned int Index(ui64 combination, int nOneBits, int nTotalBits);
  ui64 Combination(unsigned int index, int nOneBits, int nTotalBits);
};

