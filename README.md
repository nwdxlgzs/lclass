# lclass
Advanced Lua object-oriented system framework

# version support
> lua54

# lib-content

|     function/field     |                          parameters                          |                                info                                |
| :--------------------: | :----------------------------------------------------------: | :----------------------------------------------------------------: |
|        newClass        |                        类名（string）                        |                             定义一个类                             |
|   newClassWithSuper    |                类名（string）,类（luaclass）                 |                 定义一个类顺便调用setSuper设置父类                 |
|     setConstructor     |             类（luaclass）,构建函数（function）              |                 定义这个类的构建函数，只有一次机会                 |
|    setDeconstructor    |      类/对象（luaclass/luaobject）,析构函数（function）      | 如果是类定义类的析构函数，如果是对象定义对象析构函数，只有一次机会 |
| setObjectDeconstructor |            对象（luaobject）,析构函数（function）            |                   定义对象析构函数，只有一次机会                   |
|     setStaticField     |  类/对象（luaclass/luaobject）,可见性修饰（number），键，值  |           定义静态字段，定义之后会记录键值，只有一次机会           |
|        setField        |  类/对象（luaclass/luaobject）,可见性修饰（number），键，值  |           定义实例字段，定义之后会记录键值，只有一次机会           |
|    setStaticMethod     | 类/对象（luaclass/luaobject）,可见性修饰（number），键，函数 |           定义静态方法，定义之后会记录键值，只有一次机会           |
|       setMethod        | 类/对象（luaclass/luaobject）,可见性修饰（number），键，函数 |           定义实例方法，定义之后会记录键值，只有一次机会           |
|        setSuper        | 类/对象（luaclass/luaobject）, 类/对象（luaclass/luaobject） |           定义父类，只有一次机会，类不可以定义对象为父类           |
|        getSuper        |                类/对象（luaclass/luaobject）                 |                      返回父类（无继承为nil）                       |
|        getChild        |                      对象（luaobject）                       |             返回当前对象继承子类对象（叶子对象时nil）              |
|       getMethods       |                类/对象（luaclass/luaobject）                 |                  获取当前类或对象定义的public方法                  |
|       getFields        |                类/对象（luaclass/luaobject）                 |                  获取当前类或对象定义的public字段                  |
|   getDeclaredMethods   |                类/对象（luaclass/luaobject）                 |                   获取当前类或对象定义的所有方法                   |
|   getDeclaredFields    |                类/对象（luaclass/luaobject）                 |                   获取当前类或对象定义的所有字段                   |
|       lockdefine       |                类/对象（luaclass/luaobject）                 |              锁定定义，用于完成构建类或对象后锁定定义              |
|          cast          |               对象（luaobject）,类（luaclass）               |             将当前对象转为目标类的对象（失败抛出错误）             |
|       instanceof       |               对象（luaobject）,类（luaclass）               |           判断当前对象是否（或其父/子类）为目标类的对象            |
|         public         |                              -                               |         可见性修饰，不影响使用，影响后续基于此值设计的函数         |
|        private         |                              -                               |         可见性修饰，不影响使用，影响后续基于此值设计的函数         |

如果你的Lua为魔改版本，可以精确操控CallInfo、编译时准确携带额外上值信息，public和private属性也可以模拟出来（上一CallInfo的func上值包含当前类时判定为类内，允许使用private），不过这很麻烦。

## 下面是我使用此库魔改Lua实现的面向对象

```lua
--律动lua-1.0.20以后支持本语法
local class 生物{
  @static public 生物实例总数=0;
  public 名字;
  public __init__(名字){
    self.名字=名字;
    self.生物实例总数=self.生物实例总数+1
  }
}
local class 动物 extends 生物{
  public 叫声;
  public __init__(名字){
    print(@f"构建{名字}中")
  }
  public bark(){
    print(@f"{self.名字}：{self.叫声??\"还没设置叫声\"}")
  }
}
local class 猫 extends 动物{
  public __init__(名字){
    self.叫声="喵喵喵"
  }
  @meta public __mul(self,other){
    if other instanceof 猫 then
      return 猫(@f"{self.名字}*{other.名字}")
     else
      return nil
    end
  }
  @meta public __tostring(self){
    return @f"{self.名字}猫"
  }
}
class 狗 extends 动物{
  public __init__(名字){
    super.叫声="汪汪汪"
  }
  @meta public __tostring(self){
    return @f"{self.名字}狗"
  }
}
dog1=狗("柯基")
dog1:bark()
dog2=狗("德牧")
dog2:bark()
cat1=猫("加菲")
cat1:bark()
cat2=猫("暹罗")
cat2:bark()
print(生物.生物实例总数)
cat3=cat1*cat2
print(cat3)
print(生物.生物实例总数)
cat4=(cat1*dog1)??"杂交失败"
print(cat4)
print(生物.生物实例总数)
print(dog1)
a=cast<猫>dog1
```
