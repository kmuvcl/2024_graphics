./main.cpp			실습용 코드
./Makefile			Makefile
./skeleton_codes		연습 코드가 있는 디렉토리
./complete_codes		완성된 코드가 있는 디렉토리

./complete_codes/cpu_mem    프레임마다 CPU 메모리에서 GPU 메모리로 렌더링 데이터 전송 버전
./complete_codes/cpu_mem/triangle_soup.cpp      Triangle Soup 기반 렌더링 코드
./complete_codes/cpu_mem/vlist_triangles.cpp    Vertex list & Triangles 기반 렌더링 코드

./complete_codes/gpu_mem    GPU 메모리를 이용한 렌더링 버전
./complete_codes/gpu_mem/triangle_soup.cpp      VBO를 이용한 triangle Soup 기반 렌더링 코드
./complete_codes/gpu_mem/vlist_triangles.cpp    VBO와 IBO를 이용한 vertex list & triangles 기반 렌더링 코드

./doc/CG_Lab_01_Hello_Triangle.pdf          기초 실습 문서
./doc/CG_Lab_01_Hello_Triangle_Adv.pdf      심화 이론/실습 문서


# 주의사항
complete_codes/ 폴더 아래의 코드를 예제로 돌려보려면 아래와 같이 현재 디렉토디의 main.cpp에 덮어쓴 다음 make를 수행하고 실행하세요. (* 소스코드가 ./shader/ 폴더 아래의 쉐이더 코드를 참조하도록 작성되었기 때문입니다.) 

예시:
$ cp ./complete_codes/gpu_mem/triangle_soup.cpp ./main.cpp
$ make
$ ./triangle