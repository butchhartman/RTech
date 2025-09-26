import subprocess

subprocess.run(["cmake", "-S", "../.", "-B", "../build", "-G", "Ninja Multi-Config", "-DCMAKE_C_COMPILER=clang", "-DCMAKE_EXPORT_COMPILE_COMMANDS=1", "-DBUILD_TESTS=OFF"])

