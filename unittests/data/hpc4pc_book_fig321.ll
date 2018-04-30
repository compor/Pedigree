
define void @foo() #0 {
entry:
  %k = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 0, i32* %k, align 4
  store i32 0, i32* %i, align 4
  %0 = load i32, i32* %k, align 4
  %cmp = icmp sgt i32 %0, 0
  br i1 %cmp, label %if.then, label %if.end.6

if.then:                                          ; preds = %entry
  br label %while.cond

while.cond:                                       ; preds = %if.end, %if.then
  %1 = load i32, i32* %k, align 4
  %cmp1 = icmp slt i32 %1, 15
  br i1 %cmp1, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %2 = load i32, i32* %k, align 4
  %cmp2 = icmp slt i32 %2, 10
  br i1 %cmp2, label %if.then.3, label %if.else

if.then.3:                                        ; preds = %while.body
  %3 = load i32, i32* %k, align 4
  %inc = add nsw i32 %3, 1
  store i32 %inc, i32* %k, align 4
  br label %if.end

if.else:                                          ; preds = %while.body
  %4 = load i32, i32* %k, align 4
  %add = add nsw i32 %4, 3
  store i32 %add, i32* %k, align 4
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then.3
  %5 = load i32, i32* %k, align 4
  %inc4 = add nsw i32 %5, 1
  store i32 %inc4, i32* %k, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
  br label %g_label

g_label:                                          ; preds = %while.end
  %6 = load i32, i32* %k, align 4
  %add5 = add nsw i32 %6, 2
  store i32 %add5, i32* %k, align 4
  br label %if.end.6

if.end.6:                                         ; preds = %g_label, %entry
  br label %h_label

h_label:                                          ; preds = %if.end.6
  %7 = load i32, i32* %k, align 4
  %inc7 = add nsw i32 %7, 1
  store i32 %inc7, i32* %k, align 4
  ret void
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

