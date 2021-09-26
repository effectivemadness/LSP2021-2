# Linux System Programming
## Homework 01
### Makefile 조사 및 Line별 comment
---
+ Submit Path : /home/lsp30/HW1/Makefile

---

+ wildcard pattern
  - pattern에 맞는 모든 파일들을 리스팅
  - ex. \$(wildcard ./src/*.c) : src 디렉토리 아래 모든 *.c 파일 리스팅
+ addsuffix suffix, str
  - str 뒤에 suffix 붙이기
  - ex. \$(addsuffix .c, $(SRCBASENAME)) : SRCBASENAME 들에 .c 붙이기
  - addprefix도 같은 사용법 - 접두사 앞에 붙이기
+ basename str
  - str에서 '.' 과 확장자를 제외한 경로 구하기
+ notdir str
  - str에서 디렉토리가 아닌 부분만 구하기
+ foreach var, str1, str2 ... , extstr
  - 변수명에 대입 문자열(str1, str2 ...) 들을 각각 extstr에 대입
  - ex. \$(foreach x, main, ftn1, ftn2, $(x).c) : main.c, ftn1.c, ftn2.c

+ phony 규칙
  - make는 타겟을 확인할때, 파일이 있는지, 언제 수정되었는지를 확인해 rule을 실행할지 말지 결정한다. 이 때,  파일에 정의된 규칙 이름과 같은 파일이 해당 경로에 있으면 그 타겟이 만족하는 것으로 인식, 실행하지 않을 수 있다. 
  - 이 문제를 피하기 위해, 파일의 유무와 상관없이 규칙이름으로만 인식하도록 하는게 phony 규칙이다.
