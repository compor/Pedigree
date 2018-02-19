
@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: nounwind uwtable
define void @foo(i32 %n) #0 {
entry:
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %i.0 = phi i32 [ 1, %entry ], [ %add, %while.body ]
  %k.0 = phi i32 [ 0, %entry ], [ 1, %while.body ]
  %j.0 = phi i32 [ 2, %entry ], [ %mul, %while.body ]
  %cmp = icmp sle i32 %i.0, %n
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %mul = mul nsw i32 %j.0, 2
  %add = add nsw i32 %i.0, 1
  br label %while.cond

while.end:                                        ; preds = %while.cond
  %tobool = icmp ne i32 %k.0, 0
  br i1 %tobool, label %if.then, label %if.else

if.then:                                          ; preds = %while.end
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %j.0)
  br label %if.end

if.else:                                          ; preds = %while.end
  %add1 = add nsw i32 %i.0, 1
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

declare i32 @printf(i8*, ...) #1

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

