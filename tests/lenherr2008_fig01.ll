; RUN: rm -f %bindir/tests/mdg.foo2.dot
; RUN: opt -load %bindir/%testeelib -basicaa -pedigree-mdg-dfs-enumerate -pedigree-mdg-dot < %s --disable-output
; RUN: %checkgraphiso %bindir/tests/mdg.foo2.dot %outputdatadir/lenherr2008_fig01.dot | FileCheck %s
; CHECK: OK

define i32 @foo2() {
; CFG DFS preorder: 0
entry:
  %x = alloca i32*, align 8
  %y = alloca i32*, align 8
  %call = call i32* (...) @initialize()
  store i32* %call, i32** %x, align 8
  %call1 = call i32* (...) @initialize()
  store i32* %call1, i32** %y, align 8
  %0 = load i32*, i32** %x, align 8
  store i32 42, i32* %0, align 4
  %call2 = call i32 (...) @condition()
  %tobool = icmp ne i32 %call2, 0
  br i1 %tobool, label %if.then, label %if.end

; CFG DFS preorder: 1
if.then:                                          ; preds = %entry
  %1 = load i32*, i32** %y, align 8
  store i32 0, i32* %1, align 4
  br label %if.end

; CFG DFS preorder: 2
if.end:                                           ; preds = %if.then, %entry
  %2 = load i32*, i32** %x, align 8
  %3 = load i32, i32* %2, align 4
  ret i32 %3
}

declare i32* @initialize(...)

declare i32 @condition(...)
