#    NES - MOS 6502 Emulator
#    Copyright (C) 2021 JDavid(Blackhack) <davidaristi.0504@gmail.com>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/955c7f837efad184ec63e771c42542d37545eaef.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  UnitTesting
  MemoryTest.cpp
  LoadStoreTest.cpp
  RegisterTransfersTest.cpp
  StackOperationsTest.cpp
  LogicalTest.cpp
  ArithmeticTest.cpp
  IncrementsDecrementsTest.cpp
)
target_link_libraries(
  UnitTesting
  gtest_main
  NESELIB
)

include(GoogleTest)
gtest_discover_tests(UnitTesting)

include_directories(${CMAKE_SOURCE_DIR}/NESE)
