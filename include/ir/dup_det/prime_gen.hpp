#ifndef PRIME_GEN_HPP
#define PRIME_GEN_HPP


#include <types.h>
#include <ir/dup_det/integer_dyn_array.hpp>

NS_IZENELIB_IR_BEGIN

template <
  uint32_t IN_MEM_SIZE = 1000000
  >
class PrimeGen
{
  typedef izenelib::am::IntegerDynArray<uint32_t> Vector32;
  
protected:
  Vector32 primes_;
  uint32_t num_;
  uint32_t start_i_;
  uint32_t i_;
  
  FILE* f_;

public:
  inline PrimeGen(const char* fnm="./prime_num.dat")
  {
    f_ = fopen(fnm, "r");
    if (f_ == NULL)
    {
      std::cout<<"Can't create file: "<<fnm<<std::endl;
      return;
    }
    
    start_i_ = 0;
    i_ = 0;
    primes_.reserve(IN_MEM_SIZE);
    load();
  }

  inline ~PrimeGen()
  {
    fclose(f_);
  }

  inline void load()
  {
    fread(&num_, sizeof(uint32_t), 1, f_);
    load_from(0);
  }

  inline void load_from(uint32_t n = 0)
  {
    fseek (f_, (n+1)*sizeof(uint32_t), SEEK_SET);
    start_i_ = n;
    uint32_t s = IN_MEM_SIZE < num_ - start_i_? IN_MEM_SIZE: num_-start_i_;
    fread(primes_.array(s), sizeof(uint32_t)*s, 1, f_);
  }

  inline uint32_t next()
  {
    assert(i_<num_);
    
    if (i_-start_i_ >= IN_MEM_SIZE)
      load_from(start_i_ + i_);
    return primes_.at(i_++);
  }

  inline uint32_t operator [] (uint32_t i)
  {
    if (i<start_i_ || i>=start_i_+IN_MEM_SIZE)
      load_from(i);
    return primes_.at(i-start_i_);
  }
  
}
  ;

NS_IZENELIB_IR_END
#endif