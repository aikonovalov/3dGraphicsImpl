#include "../MathUtils/Plane.h"

#include <catch2/catch_test_macros.hpp>
#include <queue>

namespace testing {

TEST_CASE("Clipping correction", "[Plane]") {
  Linear::Plane plane =
      Linear::Plane({0, 0, -2, 0}, {5, 0, 0, 0}, {-10, 10, 0, 0});
  std::queue<Linear::Triangle> clip_pool;
  clip_pool.push({{0, 10, 0, 0}, {0, -10, 0, 0}, {0, 0, 20, 0}});
  plane.ClipThrough(clip_pool);

  REQUIRE(clip_pool.size() == 2);
  while (!clip_pool.empty()) {
    auto elem = clip_pool.front();
    clip_pool.pop();
    for (int vertex = 0; vertex < 3; ++vertex) {
      for (int coord = 0; coord < 3; ++coord) {
        std::cout << elem(vertex)(coord) << " ";
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  }
}
}  // namespace testing
