#include <stduuid/uuid.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <functional>
#include <iostream>
#include <random>
#include <ranges>

int main()
{
  std::random_device rd {};
  auto seedData = std::array<int, std::mt19937::state_size> {};
  std::generate(std::begin(seedData), std::end(seedData), std::ref(rd));
  std::seed_seq seq(std::begin(seedData), std::end(seedData));
  std::mt19937 generator { seq };
  uuids::uuid_random_generator uuidGenerator { generator };

  using namespace uuids;
  for (auto _ : std::views::iota(0, 10)) {
    uuid currentUuid = uuidGenerator();
    std::cout << currentUuid << "\n";
    uuid const testUuid =
      uuid::from_string(uuids::to_string(currentUuid)).value();
    assert(currentUuid == testUuid);
  }
}
