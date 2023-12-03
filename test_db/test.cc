#include <iostream>
#include <unistd.h>
#include <string>
#include <mysql/mysql.h>

const std::string host ="127.0.0.1";
const std::string user="wph";
const std::string passwd="123456";
const std::string db="conn";
const unsigned int port=8080;

using std::cin;
using std::cout;
using std::endl;
using std::cerr;

int main()
{
    // cout<<"mysql client server"<<mysql_get_client_info()<<endl;
    
    MYSQL *my=mysql_init(nullptr);
    if(nullptr==my)
    {
        cerr<<"init MySQL error"<<endl;
    }
    if(mysql_real_connect(my,host.c_str(),user.c_str(),passwd.c_str(),db.c_str(),port,nullptr,0)==nullptr)
    {
        cerr<<"connect MySQL error"<<endl;
        return 2;     
    }
    mysql_set_character_set(my,"utf8");

    // std::string sql="update user set name='jimmy' where id=2";
    // std::string sql="insert into user(name, age, telphone) values('张三',19,'165')";
    // std::string sql="select * from user where id =3";
    std::string sql="select * from user ";
    int n=mysql_query(my,sql.c_str());
    if(n==0)cout<<sql<<" success"<<endl;
    else 
    {
        cout<<sql<<" failed"<<endl;
        return 3;
    }

    MYSQL_RES *res=mysql_store_result(my);
    if(nullptr==res)
    {
        cerr<<"mysql_store_result error"<<endl;
        return 4;
    }
    //全部都是和结果集有关的，MYSQL_RES
    my_ulonglong rows=mysql_num_rows(res);
    my_ulonglong fields=mysql_num_fields(res);

    cout<<"行："<<rows<<endl;
    cout<<"列："<<fields<<endl;
    //属性：列名称
    MYSQL_FIELD *fields_array=mysql_fetch_field(res);
    for(int i=0;i<fields;i++)
    {
        cout<<fields_array[i].name<<"\t";
    }
    cout<<endl;

    //内容
    for(int i=0;i<rows;i++)
    {
        MYSQL_ROW row=mysql_fetch_row(res);
        for(int j=0;j<fields;j++)
        {
            cout<<row[j]<<"\t";
        }
        cout<<endl;
    }

    // cout<<fields_array[0].db<<" "<<fields_array[0].table<<endl;
    // std::string sql;
    // while(1)
    // {
    //     cout<<"MySQL>>> ";
    //     if(!getline(cin,sql)||sql=="quit")
    //     {
    //         cout<<"bye bye"<<endl;
    //         break;
    //     }
    //     int n=mysql_query(my,sql.c_str());
    //     if(n==0)
    //     {
    //         cout<<sql<<" success: "<<n<<endl;
    //     }
    //     else 
    //     {
    //         cerr<<sql<<" failed: "<<n<<endl;
    //     }
    // }
    

    // sleep(10);
    // cout<<"connect success"<<endl;
    mysql_free_result(res);
    mysql_close(my);
    return 0;
}