# 빌드에 필요한 패키지

## pkg-config
`pkg-config`는 Linux 시스템에 설치된 외부 라이브러리(예: GLEW, GLFW, Assimp 등)에 대한 include 경로나 library 링크 옵션 등을 손쉽게 알아낼 수 있도록 도와주는 프로그램이다. 

여기서는 `make`를 이용한 프로그램의 빌드를 편리하게 하기 위해 `Makefile` 파일에서 `pkg-config`를 사용하고 있으니, 프로그램 빌드에 앞서 아래와 같이 `pkg-config` 패키지를 설치하도록 한다.

### Ubuntu 

Ubuntu 환경에서는 다음과 같이 `pkg-config`를 설치한다.

```shell
sudo apt install pkg-config
```

