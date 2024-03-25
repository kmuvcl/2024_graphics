# 개발 정보 

Ubuntu LTS 18.04+ 운영체제가 아닌 Windows, Mac, 기타 Linux 배포판 등은 컴퓨터그래픽스 교과목의 비공식 운영체제입니다. 

> 주의: 비공식 운영체제에 대해서 본 교과목은 공식적인 기술지원을 하지 않습니다.


## 비공식 운영체제에서 가상환경으로 개발하기

비공식 운영체제에서 공식 운영체제인 Ubuntu LTS 18.04+를 쓰는 가장 편한 방법은 VirtualBox, VMWare 등을 이용해 가상머신을 이용하는 것입니다. 즉, 여려분이 쓰는 Ubuntu가 아닌 운영체제를 host로 하고 가상 환경에 Ubuntu LTS 18.04+를 guest로 설치하는 방식입니다.

과제 수행을 위한 환경을 구축하기 제일 간단한 접근 방식입니다. 비공식 운영체제를 쓰고 있다면 이 방법을 쓰는 것을 가장 권장합니다.


## 비공식 운영체제에서 직접 개발하기

학생들의 개발 편의를 위해 비공식 운영체제에서도 개발할 수 있는 정보를 제공하니 참고하기 바랍니다.

> 주의: 비공식 운영체제에서 개발하더라도 최종 평가는 공식 운영체제에서 빌드되고 구동되는 프로그램으로 평가 받습니다.

* Windows 환경
  * WSL2 (Windows Subsystem for Linux)
    * Windows 아래에 Linux 배포판을 올리는 방식입니다. WSL2 설치는 여기를 참고하세요.
    * Micrisoft Store를 통해 Ubuntu LTS 최신 배포판을 설치하세요.
    * WSL2 환경에서 구동시킨 OpenGL 프로그램을 Windows에서 띄우려면 [VcXsrv Windows X Server](https://sourceforge.net/projects/vcxsrv/)를 설치해야 합니다.
  * MinGW
    * MSYS2 + MinGW로 Windows에서 Linux 스타일의 개발환경에서 구축이 가능합니다.
    * 빌드된 결과물은 Windows Native 코드로 만들어지기 때문에 따로 X Server를 설치할 필요가 없습니다.
    * 자세한 사항은 [여기](https://github.com/kmuvcl/kmuvcl/blob/master/dev/GCC%20with%20MinGW%20(%2B%20OpenGL).md)를 참고하세요.
* Mac 환경
  * M1, M2 맥 환경 (TODO)
    * Mac을 사용하는 학생들끼리 슬랙을 통해 활발히 개발정보를 교환해 주세요.
  