# Custom shell implementation in C

[![C](https://img.shields.io/badge/C-99-A8B9CC?style=flat-square&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C99)
[![POSIX](https://img.shields.io/badge/POSIX-Compliant-orange?style=flat-square)](https://pubs.opengroup.org/onlinepubs/9699919799/)
[![Readline](https://img.shields.io/badge/GNU-Readline-red?style=flat-square)](https://tiswww.case.edu/php/chet/readline/rltop.html)
[![License](https://img.shields.io/badge/License-MIT-blue?style=flat-square)](LICENSE)

POSIX 호환 쉘 구현, C 언어로 작성. 입력을 추상 구문 트리(AST)로 파싱하고, 파이프라인, 리다이렉션, 환경 변수 확장, 작업 제어를 지원하여 명령을 실행합니다.

---

## 목차

- [개요](#개요)
- [기능](#기능)
- [파싱 파이프라인](#파싱-파이프라인)
- [실행 흐름](#실행-흐름)
- [프로젝트 구조](#프로젝트-구조)
- [빌드 및 실행](#빌드-및-실행)
- [사용 예제](#사용-예제)
- [내장 명령어](#내장-명령어)
- [라이선스](#라이선스)

---

## 개요

Custom shell은 bash의 핵심 기능을 복제하여 완전한 쉘 인터프리터를 처음부터 구현합니다. 쉘은 토큰화, 어휘 분석, AST 구성을 통해 사용자 입력을 처리한 후, 적절한 I/O 리다이렉션과 프로세스 관리를 통해 명령을 실행합니다.

**주요 기능:**

- AST 기반 명령 파싱
- 임의 깊이의 파이프라인 실행
- I/O 리다이렉션 (<, >, >>, <<)
- 환경 변수 확장 ($VAR, $?)
- 와일드카드 확장 (글롭 패턴)
- 시그널 처리 (SIGINT, SIGQUIT)
- 괄호를 사용한 서브쉘 실행
- 논리 연산자 (&&, ||)

---

## 기능

### 파싱

| 기능 | 설명 |
|------|------|
| 토큰화 | 따옴표 처리를 포함한 문자별 분류 |
| 어휘 분석 | 토큰 시퀀스를 AST 노드로 변환 |
| 구문 검증 | 닫히지 않은 따옴표, 잘못된 연산자에 대한 오류 감지 |
| AST 구성 | 연산자 우선순위를 위한 트리 구조 |

### 확장

| 기능 | 설명 |
|------|------|
| 환경 변수 | `$VAR`를 환경 값으로 교체 |
| 종료 상태 | `$?`를 마지막 명령 종료 코드로 교체 |
| 틸드 | `~`를 `$HOME`으로 확장 |
| 와일드카드 | `*` 패턴을 디렉터리 항목과 매칭 |
| 따옴표 처리 | 작은따옴표는 확장을 방지, 큰따옴표는 `$` 허용 |

### 실행

| 기능 | 설명 |
|------|------|
| 간단한 명령 | `fork()` + `execve()`를 통한 외부 명령 |
| 파이프라인 | 적절한 fd 관리를 통한 임의 파이프 체인 |
| 리다이렉션 | 입력 (<), 출력 (>), 추가 (>>), 히어독 (<<) |
| 논리 연산자 | 단락 평가를 통한 `&&` 및 `\|\|` |
| 서브쉘 | 자식 프로세스에서 괄호로 묶인 명령 그룹 |
| 내장 명령 | `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit` |

### 시그널

| 시그널 | 대화형 모드 | 실행 모드 |
|--------|-------------|----------|
| SIGINT (Ctrl+C) | 새 프롬프트 | 자식 종료 |
| SIGQUIT (Ctrl+\\) | 무시 | 코어 덤프 메시지 |
| EOF (Ctrl+D) | 쉘 종료 | - |

---

## 파싱 파이프라인

```
+------------------------------------------------------------------+
|                    Input processing flow                         |
+------------------------------------------------------------------+

    "ls -la | grep foo > out.txt && echo done"
                         |
                         v
+------------------------------------------------------------------+
| [1] Tokenization                                                 |
|     Classify each character into token types                     |
|                                                                  |
|     data: l s   - l a   |   g r e p   f o o   > ...              |
|     info: W W S W W W S O S W W W W S W W W S O ...              |
|           (W=word, S=space, O=operator)                          |
+------------------------------------------------------------------+
                         |
                         v
+------------------------------------------------------------------+
| [2] Lexing                                                       |
|     Convert token sequence into AST nodes (linked list)          |
|                                                                  |
|     [ls -la] -> [|] -> [grep foo] -> [> out.txt] -> [&&] -> ...  |
|       CMD      PIPE       CMD          REDIR       LOGIC         |
+------------------------------------------------------------------+
                         |
                         v
+------------------------------------------------------------------+
| [3] Expansion                                                    |
|     Handle $VAR, ~, * expansions                                 |
|                                                                  |
|     $HOME -> /home/user                                          |
|     *.txt -> file1.txt file2.txt                                 |
+------------------------------------------------------------------+
                         |
                         v
+------------------------------------------------------------------+
| [4] Parsing                                                      |
|     Build tree structure based on operator precedence            |
|                                                                  |
|                    [&&]                                          |
|                   /    \                                         |
|                [|]      [echo done]                              |
|               /   \                                              |
|        [ls -la]   [grep foo] -> [> out.txt]                      |
+------------------------------------------------------------------+
                         |
                         v
+------------------------------------------------------------------+
| [5] Execution                                                    |
|     Traverse AST and execute commands                            |
+------------------------------------------------------------------+
```

### 토큰 타입

| 토큰 | 코드 | 설명 |
|------|------|------|
| T_WORD | 1 | 일반 문자 |
| T_OPERATOR | 2 | 파이프, 리다이렉션, 논리 연산자 |
| T_SINGLE_QUOTE | 3 | 작은따옴표 문자 |
| T_DOUBLE_QUOTE | 4 | 큰따옴표 문자 |
| T_LITERAL | 5 | 따옴표 안의 문자 |
| T_ENV | 6 | 변수용 달러 기호 |
| T_PARENTHESIS | 7 | 서브쉘 구분자 |
| T_ASTERISK | 8 | 와일드카드 문자 |
| T_SPACE | 9 | 공백 |

---

## 실행 흐름

```
+------------------------------------------------------------------+
|                    Execution dispatch                            |
+------------------------------------------------------------------+

                     start_exec(node)
                           |
          +----------------+----------------+
          |                |                |
          v                v                v
    L_PIPELINE     L_LOGICAL_OPERATOR   L_SIMPLE_COMMAND
          |                |                |
          v                v                v
    exec_pipeline()  exec_logical()    exec_command()
          |                |                |
          v                v                v
    +----------+     +----------+     +----------+
    | fork()x2 |     | Execute  |     | Builtin? |
    | pipe()   |     | left     |     +----+-----+
    | dup2()   |     | Check    |          |
    +----------+     | result   |     Yes  |  No
          |          | Execute  |      |   |
          v          | right?   |      v   v
    child1: left     +----------+   Builtin   fork()
    child2: right                            execve()
```

### 파이프라인 실행

```
    cmd1 | cmd2 | cmd3
    
    +--------+     pipe1      +--------+     pipe2      +--------+
    | child1 |  ==========>   | child2 |  ==========>   | child3 |
    |  cmd1  |  stdout->fd[1] |  cmd2  |  stdout->fd[1] |  cmd3  |
    |        |                |        |                |        |
    +--------+                +--------+                +--------+
       |                         |                         |
       v                         v                         v
    dup2(pipe1[1], 1)         dup2(pipe1[0], 0)         dup2(pipe2[0], 0)
                              dup2(pipe2[1], 1)
```

---

## 프로젝트 구조

```
+-- includes/
|   +-- minishell.h           # main header with all declarations
|
+-- libft/                    # custom C library
|   +-- libft.h
|   +-- *.c
|
+-- srcs/
|   +-- minishell.c           # main REPL loop
|   |
|   +-- parse/
|   |   +-- tokenize.c        # character classification
|   |   +-- lexicize.c        # convert tokens to AST nodes
|   |   +-- parser.c          # AST tree construction
|   |   +-- syntax_check.c    # syntax validation
|   |   +-- syntax_cmd.c      # command-specific syntax
|   |   +-- parsing_sort.c    # node sorting
|   |
|   +-- expand/
|   |   +-- expand.c          # variable expansion
|   |   +-- expand_asterisk.c # glob pattern matching
|   |   +-- asterisk_filter.c # pattern filtering
|   |   +-- expand_heredoc.c  # heredoc expansion
|   |   +-- expand_split.c    # word splitting
|   |
|   +-- exec/
|   |   +-- exec_set_start.c  # execution entry point
|   |   +-- exec_pipeline.c   # pipe execution
|   |   +-- exec_logical_operator.c  # && || handling
|   |   +-- exec_subshell.c   # parentheses-enclosed commands
|   |   +-- exec_redirection.c       # I/O redirection
|   |   +-- exec_simple_command.c    # command execution
|   |   +-- exec_built_in.c          # builtin dispatcher
|   |   +-- exec_built_in_cd.c       # cd implementation
|   |   +-- exec_built_in_env.c      # env implementation
|   |   +-- exec_built_in_export.c   # export implementation
|   |   +-- exec_heredoc.c           # heredoc handling
|   |
|   +-- initialize/
|   |   +-- init.c            # shell initialization
|   |   +-- envp_init.c       # environment setup
|   |   +-- signal.c          # signal handler
|   |   +-- terminal.c        # terminal setup
|   |
|   +-- utils/
|   |   +-- node_ctl.c        # AST node management
|   |   +-- memory_ctl.c      # memory management
|   |   +-- line_ctl.c        # line buffer operations
|   |
|   +-- error/
|       +-- error.c           # error handling and messages
|
+-- Makefile
```

---

## 빌드 및 실행

### 사전 요구사항

- GCC 또는 Clang 컴파일러
- GNU Readline 라이브러리
- Make

### 빌드

```bash
# 표준 빌드
make

# 오브젝트 파일 정리
make clean

# 실행 파일 포함 전체 정리
make fclean

# 재빌드
make re
```

### 실행

```bash
./minishell
```

### 의존성

프로젝트는 readline 라이브러리가 필요합니다:

```bash
# macOS (Homebrew)
brew install readline

# Ubuntu/Debian
sudo apt-get install libreadline-dev

# Fedora
sudo dnf install readline-devel
```

### 기본 명령어

```bash
minishell$ echo Hello, World!
Hello, World!

minishell$ ls -la
total 64
drwxr-xr-x  10 user  staff   320 Jan 10 12:00 .
...

minishell$ pwd
/home/user/projects/minishell
```

### 파이프라인

```bash
minishell$ ls -la | grep ".c" | wc -l
42

minishell$ cat file.txt | head -n 10 | tail -n 5
```

### 리다이렉션

```bash
minishell$ echo "Hello" > output.txt
minishell$ cat < input.txt
minishell$ echo "Append" >> output.txt
minishell$ cat << EOF
> Line 1
> Line 2
> EOF
```

### 환경 변수

```bash
minishell$ export FOO=bar
minishell$ echo $FOO
bar

minishell$ echo $?
0

minishell$ echo $HOME
/home/user
```

### 논리 연산자

```bash
minishell$ true && echo "Success"
Success

minishell$ false || echo "Fallback"
Fallback

minishell$ mkdir test && cd test && pwd
/home/user/test
```

### 서브쉘

```bash
minishell$ (cd /tmp && pwd)
/tmp
minishell$ pwd
/home/user
```

### 와일드카드

```bash
minishell$ echo *.c
main.c utils.c parser.c

minishell$ ls src/*.h
src/minishell.h
```

---

## 내장 명령어

| 명령어 | 설명 | 옵션 |
|--------|------|------|
| `echo` | 인수 출력 | `-n`: 후행 개행 없음 |
| `cd` | 디렉터리 변경 | `-`: 이전 디렉터리 |
| `pwd` | 작업 디렉터리 출력 | - |
| `export` | 환경 변수 설정 | 인수 없음: 모두 출력 |
| `unset` | 환경 변수 제거 | - |
| `env` | 환경 출력 | - |
| `exit` | 쉘 종료 | [status]: 종료 코드 |

### 예제

```bash
minishell$ echo -n "No newline"
No newlineminishell$

minishell$ cd /tmp
minishell$ cd -
/home/user

minishell$ export VAR=value
minishell$ unset VAR

minishell$ exit 42
```

---

## 라이선스

이 프로젝트는 MIT 라이선스 하에 라이선스됩니다.