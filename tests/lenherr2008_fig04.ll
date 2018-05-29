; RUN: rm -f %bindir/tests/mdg.foo3.dot
; RUN: opt -load %bindir/%testeelib -basicaa -pedigree-mdg-dfs-enumerate -pedigree-mdg-dot < %s --disable-output
; RUN: %checkgraphiso %bindir/tests/mdg.foo3.dot %outputdatadir/lenherr2008_fig04.dot | FileCheck %s
; CHECK: OK

define void @foo3() {
; CFG DFS preorder: 0
entry:
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %p1 = alloca i32*, align 8
  %p2 = alloca i32*, align 8
  %p3 = alloca i32*, align 8
  %dp = alloca i32**, align 8
  store i32* %a, i32** %p1, align 8
  store i32** %p1, i32*** %dp, align 8
  br label %while.cond

; CFG DFS preorder: 1
while.cond:                                       ; preds = %while.body, %entry
  %call = call i32 (...) @condition()
  %tobool = icmp ne i32 %call, 0
  br i1 %tobool, label %while.body, label %while.end

; CFG DFS preorder: 2
while.body:                                       ; preds = %while.cond
  %0 = load i32*, i32** %p1, align 8
  store i32* %0, i32** %p2, align 8
  %1 = load i32**, i32*** %dp, align 8
  store i32* %b, i32** %1, align 8
  br label %while.cond

; CFG DFS preorder: 3
while.end:                                        ; preds = %while.cond
  %2 = load i32**, i32*** %dp, align 8
  %3 = load i32*, i32** %2, align 8
  store i32* %3, i32** %p3, align 8
  ret void
}

declare i32 @condition(...)
