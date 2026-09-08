#pragma once
#include <iosfwd>
#include <sstream>
#include<algorithm>

#include "parser.h"
class Generator
{
public:
    Generator(node_prog* root)
        :m_root(root)
    {

    }
    void gen_scope(node_scope* scope)
    {
        scope_begin();
        for (node_st* st :scope->stmt)
        {
            gen_st(st);
        }
        scope_end();
    }
    void gen_term(node_term* term)
    {
        if (holds_alternative<node_ident*>(term->var))
        {
            node_ident* ident=get<node_ident*>(term->var);
            auto it=find_if(m_vars.begin() ,m_vars.end() ,[&](const var& v){return v.name==ident->ident.value.value();});
            if (it==m_vars.end())
            {
                cerr<<"undeclared identifier"<<ident->ident.value.value()<<endl;
                exit(EXIT_FAILURE);
            }
            stringstream offset;
            offset<<"QWORD[rsp+ "<<(m_size_stack-(*it).stack_loc-1)*8<<"]";
            push(offset.str());
        }
        else if(holds_alternative<node_number*>(term->var))
        {
            node_number* number=get<node_number*>(term->var);
            m_output<<"     mov rax , "<<number->number.value.value()<<endl;;
            push("rax");
        }
        else if (holds_alternative<node_grouped_expr*>(term->var))
        {
            node_grouped_expr* grouped =get<node_grouped_expr*>(term->var);
            gen_exp(grouped->expr);
        }
    }
    void gen_bin(node_bin* bin)
    {
        if (holds_alternative<node_bin_expr_add*>(bin->var))
        {
            node_bin_expr_add* add=get<node_bin_expr_add*>(bin->var);
            gen_exp(add->lhs);
            gen_exp(add->rhs);
            pop("rax");
            pop("rbx");
            m_output<<"     add rax , rbx\n";
            push("rax");
        }
        else if (holds_alternative<node_bin_expr_sub*>(bin->var))
        {
            node_bin_expr_sub* sub=get<node_bin_expr_sub*>(bin->var);
            gen_exp(sub->lhs);
            gen_exp(sub->rhs);
            pop("rbx");
            pop("rax");
            m_output<<"     sub rax , rbx\n";
            push("rax");
        }
        else if (holds_alternative<node_bin_expr_mult*>(bin->var))
        {
            node_bin_expr_mult* mult=get<node_bin_expr_mult*>(bin->var);
            gen_exp(mult->lhs);
            gen_exp(mult->rhs);
            pop("rax");
            pop("rbx");
            m_output<<"     imul rax , rbx\n";
            push("rax");
        }
        else if (holds_alternative<node_bin_expr_div*>(bin->var))
        {
            node_bin_expr_div* div=get<node_bin_expr_div*>(bin->var);
            gen_exp(div->lhs);
            gen_exp(div->rhs);
            pop("rbx");
            pop("rax");
            m_output << "     xor rdx, rdx\n";
            m_output << "     div rbx\n";
            push("rax");
        }
    }
    void gen_exp(node_expr* exp)
    {
        if (holds_alternative<node_bin*>(exp->var))
        {
            node_bin* bin=get<node_bin*>(exp->var);
            gen_bin(bin);
        }
        else if (holds_alternative<node_term*>(exp->var))
        {
            node_term* term=get<node_term*>(exp->var);
            gen_term(term);
        }
    }
    void gen_if_pred(node_if_pred* if_pred , string& end_lable)
    {
        if (holds_alternative<node_if_pred_else_if*>(if_pred->var))
        {
            node_if_pred_else_if* else_if=get<node_if_pred_else_if*>(if_pred->var);
            gen_exp(else_if->expr);
            pop("rax");
            string lable=create_label();
            m_output<<"     test rax , rax"<<endl;
            m_output<<"     jz "<<lable<<endl;
            gen_scope(else_if->scope);
            m_output<<"     jmp "<<end_lable<<endl;
            m_output<<lable <<":"<<endl;
            if (else_if->pred.has_value())
            {
                gen_if_pred(else_if->pred.value() , end_lable);
            }
        }
        else if(holds_alternative<node_if_pred_else*>(if_pred->var))
        {
            node_if_pred_else* pred_else=get<node_if_pred_else*>(if_pred->var);
            gen_scope(pred_else->scope);
        }

    }

    void gen_st(node_st* st)
    {
        if (holds_alternative<node_return*>(st->var))
        {
            node_return* _return=get<node_return*>(st->var);
            gen_exp(_return->expr);
            m_output<<"     mov rax, 60\n";
            pop("rdi");
            m_output<<"     syscall\n";
        }
        else if (holds_alternative<node_scope*>(st->var))
        {
            node_scope* scope=get<node_scope*>(st->var);
            gen_scope(scope);
        }
        else if (holds_alternative<node_let*>(st->var))
        {
            node_let* let =get<node_let*>(st->var);
            auto search_start=m_scope.empty()?m_vars.begin() : m_vars.begin()+m_scope.back();
            auto it=find_if(search_start , m_vars.end(), [&](const var& v){return v.name==let->ident.value.value();});
            if (it !=m_vars.end())
            {
                cerr<<"identifier is already used"<<let->ident.value.value()<<endl;
                exit(EXIT_FAILURE);
            }
            m_vars.push_back({.name = let->ident.value.value(), .stack_loc = m_size_stack});
            gen_exp(let->expr);
        }
        else if (holds_alternative<node_assignment*>(st->var))
        {
            node_assignment* assignment=get<node_assignment*>(st->var);
            auto it=find_if(m_vars.begin() , m_vars.end() , [&](const var& v){return v.name==assignment->ident.value.value();});
            if (it==m_vars.end())
            {
                cerr<<"undeclared identifier"<<assignment->ident.value.value()<<endl;
                exit(EXIT_FAILURE);
            }
            gen_exp(assignment->expr);
            pop("rax");
            m_output << "     mov [rsp + " << (m_size_stack- it->stack_loc - 1) * 8 << "], rax\n";
        }
        else if (holds_alternative<node_if*>(st->var))
        {
            node_if* _if=get<node_if*>(st->var);
            gen_exp(_if->expr);
            pop("rax");
            string label=create_label();
            m_output<<"     test rax , rax"<<endl;
            m_output<<"     jz "<<label<<endl;
            gen_scope(_if->scope);
            m_output<<label<<":"<<endl;
            if (_if->pred.has_value())
            {
                string end_label=create_label();
                gen_if_pred(_if->pred.value(), end_label);
                m_output<< end_label<<":"<<endl;
            }
        }
    }


    string generate()
    {
        m_output<<"global _start\n_start:\n";
        for (node_st* st: m_root->stmt)
        {
            gen_st(st);
        }
        m_output<<"     mov rax , 60\n";
        m_output<<"     mov rdi , 0\n";
        m_output<<"     syscall\n";
        return m_output.str();

    }
private:
    void push(const string reg)
    {
        m_output<<"     push "<<reg<<endl;
        m_size_stack++;
    }
    void pop(const string reg)
    {
        m_output<<"     pop "<<reg<<endl;
        m_size_stack--;
    }
    void scope_begin()
    {
        m_scope.push_back(m_vars.size());
    }
    void scope_end()
    {
       size_t crt=m_vars.size()-m_scope.back();
        m_output<<"     add rsp, "<<crt*8<<endl;
        m_size_stack-=crt;
        for (int i=0 ; i<crt ;++i)
        {
            m_vars.pop_back();
        }
        m_scope.pop_back();
    }
    string create_label()
    {
        return "label" +to_string(m_label_crt++);
    }
    struct var
    {
        string name;
        size_t stack_loc;
    };
    node_prog* m_root;
    size_t m_size_stack=0;
    size_t m_label_crt = 0;
    stringstream m_output;
    vector<var>m_vars;
    vector<size_t>m_scope;



};
