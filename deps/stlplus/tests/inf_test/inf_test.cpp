#include "inf.hpp"
#include "timer.hpp"
#include "build.hpp"
#include "file_system.hpp"
#include "persistent_inf.hpp"
#include "persistent_shortcuts.hpp"
#include "strings.hpp"
#include <map>
#include <stdio.h>
#include <stdlib.h>

#define DATA "inf_test.tmp"
#define MASTER "inf_test.dump"

#ifdef _WIN32
typedef __int64 bigint;
#else
typedef long long bigint;
#endif

#define SAMPLES 100

static std::string hex_image (bigint value)
{
  char str[1000];
#ifdef _WIN32
  sprintf(str, "0x%I64x", value);
#else
  sprintf(str, "%llx", value);
#endif
  return std::string(str);
}

static std::string dec_image (bigint value)
{
  char str[1000];
#ifdef _WIN32
  sprintf(str, "%I64d", value);
#else
  sprintf(str, "%lld", value);
#endif
  return std::string(str);
}

static bool compare (bigint left, const stlplus::inf& right)
{
  std::string left_image = dec_image(left);
  std::string right_image = right.to_string(10);
  if (left_image != right_image)
  {
    std::cerr << "failed comparing integer " << left_image << " with inf " << right << std::endl;
    return false;
  }
  return true;
}

static bool report(int a, int b, const char* op)
{
  std::cerr << "operator " << op << " failed with a = " << a << ", b = " << b << std::endl;
  return false;
}

static bool test (int a, int b)
{
  bool okay = true;
  bigint int_a(a);
  stlplus::inf inf_a(a);
  bigint int_b(b);
  stlplus::inf inf_b(b);

  if (!compare((int_a + int_b), (inf_a + inf_b)))
    okay &= report(a, b, "+");
  if (!compare((int_a - int_b), (inf_a - inf_b)))
    okay &= report(a, b, "-");
  if (!compare((int_a * int_b), (inf_a * inf_b)))
    okay &= report(a, b, "*");
  if (b != 0)
  {
    if (!compare((int_a / int_b), (inf_a / inf_b)))
      okay &= report(a, b, "/");
    if (!compare((int_a % int_b), (inf_a % inf_b)))
      okay &= report(a, b, "%");
  }
  if (!compare((int_a | int_b), (inf_a | inf_b)))
    okay &= report(a, b, "|");
  if (!compare((int_a & int_b), (inf_a & inf_b)))
    okay &= report(a, b, "&");
  if (!compare((int_a ^ int_b), (inf_a ^ inf_b)))
    okay &= report(a, b, "^");
  for (unsigned shift = 1; shift < 16; shift*=2)
  {
    if (!compare((int_a << shift), (inf_a << shift)))
      okay &= report(a, shift, "<<");
    if (!compare((int_a >> shift), (inf_a >> shift)))
      okay &= report(a, shift, ">>");
  }
  for (unsigned high = 1; high < 16; high*=2)
  {
    for (unsigned low = 0; low < high+1; low++)
    {
      stlplus::inf inf_a(a);
      if (high < inf_a.size())
      {
        // discard the lsbs by simply shifting right
        bigint bigint_slice = int_a >> low;
        // mask the msbs and sign extend the result
        int length = int(high)-int(low)+1;
        if (bigint_slice & (bigint(1) << (length-1)))
          bigint_slice |= ((~bigint(0)) >> length) << length;
        else
          bigint_slice &= ~(((~bigint(0)) >> length) << length);
        stlplus::inf slice = inf_a.slice(low,high);
        if (!compare(bigint_slice, slice))
        {
          std::cerr << "slice failed with hex integer = " << hex_image(int_a) << ", high = " << high << ", low = " << low << std::endl;
          std::cerr << "  inf slice = " << slice.image_debug() << ", integer slice = " << hex_image(bigint_slice) << std::endl;
          okay &= false;
        }
      }
    }
  }
  return okay;
}

int main (int argc, char* argv[])
{
  bool okay = true;
  stlplus::timer cpu_time;
  std::cerr << "build: " << stlplus::build() << std::endl;
  try
  {
    srand((unsigned)time(0));
    signed long samples = (argc > 2) ? ((argc-1)/2) : argc == 2 ? atol(argv[1]) : SAMPLES;
    std::cerr << "testing " << samples << " samples" << std::endl;
    double cpu = cpu_time.cpu();
    for (signed long i = 1; i <= samples; i++)
    {
      int a = (argc > 2) ? atol(argv[i*2-1]) : rand();
      int b = (argc > 2) ? atol(argv[i*2]) : rand();
      std::cerr << "#" << i << " a = " << a << " b = " << b << std::endl;
      okay &= test(a, b);
      okay &= test(-a, b);
      okay &= test(a, -b);
      okay &= test(-a, -b);
      if (cpu_time.cpu() > (cpu + 5.0))
      {
        cpu = cpu + 5.0;
        std::cerr << cpu_time << ": tested " << i << " samples" << std::endl;
      }
    }
    std::cerr << cpu_time << " total for " << samples << " samples" << std::endl;

    // test the ability to represent a googol in all the possible radices
    // pre-load the results
    std::map<int,std::string> results;
    results[2] = std::string("0b0100100100100110101101001001011001010011000011011111001110101100001011001001111000010011000100110011100000101111110011100010101100111001000000100011100010000100011010011111001010101010110010010000110000100010101000001011101000111100010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    results[3] = std::string("3#122012210112120112111212010011100001101211222101110010100012001010011011021010111212020100220020021122002200200010101000112122102122010002012010000000120120022011020201122101010221121011200012121021202022020101");
    results[4] = std::string("4#10210212231021121103003133032230023021320103010303200233303202230321000203202010122133022222302100300202220023220330100000000000000000000000000000000000000000000000000");
    results[5] = std::string("5#102414221203323202133113331031102220100330010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    results[6] = std::string("6#225421520555002405205021514331412332420034405541334340545305400514013453222020504101024233010145312244402025311325050123301452144");
    results[7] = std::string("7#16201341553122251063252024261246503522112115506446252526241360534151125226544036056624134325461423451523416401660341314");
    results[8] = std::string("00444465511312303371654131170230463405763425471004342043237125262206042501350742000000000000000000000000000000000");
    results[9] = std::string("9#565715515455104301354871403305033134233455210806248080603330478378102163000516264221571127534605537668211");
    results[10] = std::string("10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    results[11] = std::string("11#107611186a648903471813259295629384570114251063297959276735639698066525944868937609960a3a7a17391a1");
    results[12] = std::string("12#52377002841b241b07a24b908b19a6959ab7a08120693b00991a1299b018ab70b11144a6506633388a9046ba11054");
    results[13] = std::string("13#72c7a1051523285a7244587a82c6184b210239756b3033791a1381197a005ccbbc9a54461668a53a8589393273");
    results[14] = std::string("14#1d15ba97ca20a0b65db2c537911c9cb06dc7c31084bd85a25bdb6540a6a9b3b1347c6055a2b633cbadb09b44");
    results[15] = std::string("15#11251839c078059010e1ae8598a337dcecbec6ceb0034ed2d436ec35cac6406a2de1e31c5b3d007905ce6a");
    results[16] = std::string("0x1249ad2594c37ceb0b2784c4ce0bf38ace408e211a7caab24308a82e8f10000000000000000000000000");
    results[17] = std::string("17#22b12972d2793de45bcaaae50d2a3fgcaacgd5e6d5c4937d52g8fb8d33a2a8c6d5bgbec4e420780ff4");
    results[18] = std::string("18#6ec1bcf4688309gh806h932adcc44eeg6de0fe9fahde66dgh108c9g3623e045a0h7a95ab594ce99a");
    results[19] = std::string("19#1f6dcb713d74f8gac9c5agi2e0ecb55da61fi9ae08e7hchf2b236672e128h29gfgccc8874hhc8a9");
    results[20] = std::string("20#d4dj27751e811e02jc8j525841500000000000000000000000000000000000000000000000000");
    results[21] = std::string("21#6h307dg64gac07d579f617g0014878b0gja0e6j69ddh5h541k343520iii6f48e5cdg26dfi454");
    results[22] = std::string("22#4cflf34gchk5jcc43fa5flhb96c2jf5f3572g5afh42dj31a12ga9hdjk94ah2ck1ccl4i9j0ac");
    results[23] = std::string("23#3l6adc1kjjfb6j7eh7e9d20gdg43l07ccad1a8aea3gdebi38g5da3hhkcb9aj9e7a8jca817d");
    results[24] = std::string("24#453j58dkc2beg2k1i3m4c67bba9732e6k4jeed4eg5dh5ndfenc1f0i3301c320c0dm7adf2g");
    results[25] = std::string("25#5e9c73hh28g8i5g5ca53f100000000000000000000000000000000000000000000000000");
    results[26] = std::string("26#8oikgc2dn19n3hd0gd7ch09157j49md2mcejjlj2ak237pn1p3he8p0khnn9bomijgk20gg");
    results[27] = std::string("27#h5lefedn3491amqac395134473dn69o67h2ii3a0ehbh325300ff846jha3pg4i5g7k86a");
    results[28] = std::string("28#1b5eegphn0cfn4ae1la8logkfjiq3gk2e6nik0hfnmn26i9gckdk7bh7n1q8g3i9nepk94");
    results[29] = std::string("29#3hg0h7pl6s4qb96hp43oepi8o7helm5rf6i6ngekhbsmln09c6qle0k78ee7f8mlkjmsg");
    results[30] = std::string("30#anhmc58j7lglcg33hodnk7sndpa3aj70j1rppi9f83ktbpktjjg6oft86bqa44opkb3a");
    results[31] = std::string("31#16532c98ncbeel7rel86l7845f0bhkq3cg6dq10e9o3deb14sbclrn8efmm462nsapg5");
    results[32] = std::string("32#4i9lkip9grstc5if164po5v72me827226jslap462585q7h00000000000000000000");
    results[33] = std::string("33#jq9lr1s39q0wlawtn6vwll8ob7qaskkiupbe9fhge89wfh2tn7rllf8f5afpm2g7i1");
    results[34] = std::string("34#2sn7biwwnggi76tqbhc2muect6iraeb1f6nbcr0ahi3wxxe2hm8hxdtjqeegp6ttg4");
    results[35] = std::string("35#f4aydl5wlj4rsxp7lpc21v0vb700c5bdkncfb881jck8oo7d2x7t6m0bjhouw1cfp");
    results[36] = std::string("36#2hqbczu2ow52bala8lgc3s5y9mm5tiy0vo9tke25466gfi6ax8gs22x7kuu8l1tds");
    std::cerr << "Testing a googol" << std::endl;
    // now do the test
    stlplus::inf googol(results[10]);
    std::cerr << "a googol is:" << std::endl;
    for (unsigned radix = 2; radix <= 36; radix++)
    {
      std::string converted = stlplus::inf_to_string(googol,radix);
      std::cerr << "base " << radix << " = " << converted << std::endl;
      if (converted != results[radix])
      {
        okay = false;
        std::cerr << "ERROR: different result for radix " << radix << std::endl;
        std::cerr << "  result : " << converted << std::endl;
        std::cerr << "  correct: " << results[radix] << std::endl;
      }
    }

    // test the persistence
    std::cerr << "dumping" << std::endl;
    stlplus::dump_to_file(googol, DATA, stlplus::dump_inf, 0);
    stlplus::inf googol2;
    std::cerr << "restoring" << std::endl;
    stlplus::restore_from_file(DATA, googol2, stlplus::restore_inf, 0);
    if (googol != googol2)
    {
      std::cerr << "restored value " << googol2 << " is not the same" << std::endl;
      okay = false;
    }
    // compare with the master dump if present
    if (!stlplus::file_exists(MASTER))
      stlplus::file_copy(DATA,MASTER);
    else
    {
      std::cerr << "restoring master" << std::endl;
      stlplus::inf googol3;
      stlplus::restore_from_file(MASTER,googol3,stlplus::restore_inf,0);
      if (googol != googol3)
      {
        std::cerr << "restored value " << googol3 << " is not the same" << std::endl;
        okay = false;
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "caught std::exception " << e.what() << std::endl;
    okay &= false;
  }
  catch (...)
  {
    std::cerr << "caught unknown exception " << std::endl;
    okay &= false;
  }
  std::cerr << cpu_time << " total for program" << std::endl;
  std::cerr << "test " << (okay ? "succeeded" : "FAILED") << std::endl;
  return okay ? 0 : 1;
}
