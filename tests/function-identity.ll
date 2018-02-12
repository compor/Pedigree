; RUN: opt -load %bindir/%testeelib -pedigree -S < %s | FileCheck %s

; CHECK-LABEL: foo
; CHECK: alloca i32

define i32 @foo() {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %b = load i32, i32* %1, align 4
  ret i32 %b
}

