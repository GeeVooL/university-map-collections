#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>

#include "HashMap.h"
#include "TreeMap.h"

namespace {

void perfomTest(size_t size) {
  aisdi::TreeMap<int, std::string> tmap;
  aisdi::HashMap<int, std::string> hmap;

  for (size_t i = 0; i < size; ++i) {
    tmap[i] = "tree" + std::to_string(i);
    hmap[i] = "hash" + std::to_string(i);
  }

  std::chrono::high_resolution_clock::time_point t1 =
      std::chrono::high_resolution_clock::now();
  tmap[666] = "Is this the real life?";
  std::chrono::high_resolution_clock::time_point t2 =
      std::chrono::high_resolution_clock::now();
  auto duration1 =
      std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
  std::cout << "Treemap add element time: " << duration1 << std::endl;

  std::chrono::high_resolution_clock::time_point t3 =
      std::chrono::high_resolution_clock::now();
  hmap[666] = "Is this just fantasy?";
  std::chrono::high_resolution_clock::time_point t4 =
      std::chrono::high_resolution_clock::now();
  auto duration2 =
      std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
  std::cout << "Hashmap add element time: " << duration2 << std::endl;

  std::chrono::high_resolution_clock::time_point t5 =
      std::chrono::high_resolution_clock::now();
  tmap.valueOf(666);
  std::chrono::high_resolution_clock::time_point t6 =
      std::chrono::high_resolution_clock::now();
  auto duration3 =
      std::chrono::duration_cast<std::chrono::microseconds>(t6 - t5).count();
  std::cout << "Treemap get element value: " << duration3 << std::endl;

  std::chrono::high_resolution_clock::time_point t7 =
      std::chrono::high_resolution_clock::now();
  hmap.valueOf(666);
  std::chrono::high_resolution_clock::time_point t8 =
      std::chrono::high_resolution_clock::now();
  auto duration4 =
      std::chrono::duration_cast<std::chrono::microseconds>(t8 - t7).count();
  std::cout << "Hashmap get element value: " << duration4 << std::endl;

  std::chrono::high_resolution_clock::time_point t9 =
      std::chrono::high_resolution_clock::now();
  for (auto it = tmap.cbegin(); it != tmap.cend(); ++it);
  std::chrono::high_resolution_clock::time_point t10 =
      std::chrono::high_resolution_clock::now();
  auto duration5 =
      std::chrono::duration_cast<std::chrono::microseconds>(t10 - t9).count();
  std::cout << "Treemap iterate: " << duration5 << std::endl;

  std::chrono::high_resolution_clock::time_point t11 =
      std::chrono::high_resolution_clock::now();
  for (auto it = hmap.cbegin(); it != hmap.cend(); ++it);
  std::chrono::high_resolution_clock::time_point t12 =
      std::chrono::high_resolution_clock::now();
  auto duration6 =
      std::chrono::duration_cast<std::chrono::microseconds>(t12 - t11).count();
  std::cout << "Hashmap iterate: " << duration6 << std::endl;
}
}  // namespace

int main(int argc, char** argv) {
  const std::size_t mapSize = argc > 1 ? std::atoll(argv[1]) : 10000;
  perfomTest(mapSize);
  return 0;
}
