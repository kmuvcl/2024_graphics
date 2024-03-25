./cpp			실습용 작업 디렉토리
./doc			실습 문서 디렉토리

./cpp/skeleton/		뼈대코드 디렉토리
./cpp/solution/		정답코드 디렉토리

./doc/CG_Lab_02_User_Interactions.pdf          	기초 실습 문서
./doc/CG_Lab_02_User_Interactions_Adv.pdf	심화 이론/실습 문서


# 주의사항
solution/ 아래의 정답코드를 예제로 돌려보려면 다음과 같이 main.cpp에 덮어쓴 다음 make를 수행하고 실행하세요. (* 소스코드가 ./shader/ 폴더 아래의 쉐이더 코드를 참조하도록 작성되었기 때문입니다.) 

예시:
$ cd ./cpp
$ cp ./solution/main.cpp ./main.cpp
$ make
$ ./interaction