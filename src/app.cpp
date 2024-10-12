/*
    클라이언트 코드에 클래스 타입을 몰라도 동작하는 이유는 다형성 때문이다.
    다형성은 코드 호출 시 맥락에 맞게 사용자가 정의한 코드가 호출될 수 있도록 한다.
    즉, 메모리 상에 할당된 객체를 코드가 호출되는 맥락에 따라 실제 자료형을 모르더라도 
    코드가 동작될 수 있도록 하기 때문에 CAgent* 포인터만으로 모든 작업을 수행하여 코드가 동작할 수 있다.
*/


#include <iostream>
#include "agent.hpp"
#include "mmanager.hpp"
#include "stochastic_decorator.hpp"
#include "simple_decorator.hpp"

#include <vector>
#include <string>
#include "tinyxml2.h"
#include "special_agent.hpp"

class InitManager   // 팩토리 역할
{
public:
    InitManager(){ }    // 기본 생성자
    InitManager(std::string path){  // path를 받는 생성자
        // path 경로의 XML 파일 로드
        tinyxml2::XMLDocument doc;
        doc.LoadFile( path.c_str() );
        // "scenario" 엘리먼트 하위의 "AgentList" 엘리먼트 가져옴
        tinyxml2::XMLElement* agListElem = doc.FirstChildElement( "scenario" )->FirstChildElement( "AgentList" );
        
        double x, y, heading, speed, drange;
        // "AgentList" 엘리먼트 하위의 엘리먼트를 순서대로 가져옴
        for (tinyxml2::XMLElement* agElem = agListElem->FirstChildElement();
            agElem != NULL; agElem = agElem->NextSiblingElement())
            {
                // 각 엘리먼트의 어트리뷰트에 있는 속성값들을 가져옴
                agElem->QueryDoubleAttribute("x", &x);
                agElem->QueryDoubleAttribute("y", &y);
                agElem->QueryDoubleAttribute("h", &heading);
                agElem->QueryDoubleAttribute("s", &speed);
                agElem->QueryDoubleAttribute("r", &drange);
                // 읽어온 속성값을 사용해 객체 생성 및 리스트에 추가
                CAgent* ag = new CSpecialAgent(x,  y, heading, speed, drange);
                m_agent_list.push_back(ag);
            }
    }
private:
    std::vector<CAgent*> m_agent_list;  // 에이전트 객체의 포인터를 저장하는 리스트

public:
    // m_agent_list를 사용하기 위함
    // & : 참조형. m_agent_list를 참조형으로 보내준다. 
    // 그냥 보내면 객체를 생성하여 소멸자도 호출하여 불필요하게 값이 복사되어 성능이 떨어지게 된다. 
    std::vector<CAgent*>& get_agent_list() { return m_agent_list; }
};

int main(int argc, char** argv)
{// Clinet Code  

    // XMLElement* titleElement = doc.FirstChildElement( "PLAY" )->FirstChildElement( "TITLE" );
    // XMLElement* attributeApproachElement = doc.FirstChildElement()->FirstChildElement( "attributeApproach" );
	// attributeApproachElement->QueryIntAttribute( "v", &v0 );

    InitManager init_manager("test.xml");

    /* Simulation Engine */
    CManeuverManager mmanager;

    for (std::vector<CAgent*>::iterator iter = init_manager.get_agent_list().begin();
        iter != init_manager.get_agent_list().end(); ++iter)
        {
            mmanager.register_publisher(*iter);
        }

    // CAgent* p_ag = ???.create_agent(10,  0, 0, 1, 10); // 10, 0, 0, 1, 10 은 xml에서 가져오게 하면 됨
    // m_agent_list.push_back(p_ag);

    // /* Simulation Model Registration */
    // CAgent* p_agent1 = new CAgent(10,  0, 0, 1, 10);
    // p_agent1 = new StochasticDecorator(p_agent1);
    // p_agent1 = new SimpleDecorator(p_agent1);
    
    // mmanager.register_publisher(p_agent1);

    // CAgent* p_agent2 = new CAgent(10,  30, 180, 1, 10);
    // mmanager.register_publisher(p_agent2);
    
    /* Simulation Engine Initialization */
    double sim_time;
    double time_step = 1;

    for(sim_time = 0; sim_time < 30; sim_time += time_step)
    {   
        //p_agent1->maneuver(time_step);
        //p_agent2->maneuver(time_step);

        //p_agent1->detect(p_agent2);
        //p_agent2->detect(p_agent1);

        mmanager.svc(time_step);

        // std::cout << "----" << std::endl;
        // std::cout << "Time: " << sim_time << ", " <<*p_agent1 << std::endl;
        // std::cout << "Time: " << sim_time << ", " <<*p_agent2 << std::endl;

        std::cout << "----" << std::endl;
        for (std::vector<CAgent*>::iterator iter = init_manager.get_agent_list().begin();
        iter != init_manager.get_agent_list().end(); ++iter)
        {
            std::cout << "Time: " << sim_time << ", " <<*(*iter) << std::endl;
        }
    }
    return 0;
}