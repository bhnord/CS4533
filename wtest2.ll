; ModuleID = 'WPLC.ll'
source_filename = "WPLC.ll"

@test = private unnamed_addr constant [6 x i8] c"hello\00", align 1
@const = private unnamed_addr constant [10 x i8] c"2nd thing\00", align 1

define i32 @t1() {
t1head:
  br label %enter

enter:                                            ; preds = %t1head
  %test = alloca i8*, align 8
  store i8* getelementptr inbounds ([6 x i8], [6 x i8]* @test, i32 0, i32 0), i8** %test, align 8
  store i8* getelementptr inbounds ([10 x i8], [10 x i8]* @const, i32 0, i32 0), i8** %test, align 8
  ret i32 4
}

define i32 @c(i32 %k) {
chead:
  %k1 = alloca i32, align 4
  store i32 %k, i32* %k1, align 4
  br label %enter

enter:                                            ; preds = %chead
  ret i32 5
}
