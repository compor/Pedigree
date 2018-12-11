define dso_local void @foo(i32 %a) {
entry:
  %a.addr = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  %0 = load i32, i32* %a.addr, align 4
  %tobool = icmp ne i32 %0, 0
  br i1 %tobool, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  ret void

if.else:                                          ; preds = %entry
  call void @exit(i32 1)
  unreachable
}

declare dso_local void @exit(i32)

