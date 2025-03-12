/*
 * Copyright (c) 2015 Universita' degli Studi di Napoli "Federico II"
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Pasquale Imputato <p.imputato@gmail.com>
 * Author: Stefano Avallone <stefano.avallone@unina.it>
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/traffic-control-module.h"

// This simple example shows how to use TrafficControlHelper to install a
// QueueDisc on a device.
//
// The default QueueDisc is a pfifo_fast with a capacity of 1000 packets (as in
// Linux). However, in this example, we install a RedQueueDisc with a capacity
// of 10000 packets.
//
// Network topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//
// The output will consist of all the traced changes in the length of the RED
// internal queue and in the length of the netdevice queue:
//
//    DevicePacketsInQueue 0 to 1
//    TcPacketsInQueue 7 to 8
//    TcPacketsInQueue 8 to 9
//    DevicePacketsInQueue 1 to 0
//    TcPacketsInQueue 9 to 8
//
// plus some statistics collected at the network layer (by the flow monitor)
// and the application layer. Finally, the number of packets dropped by the
// queuing discipline, the number of packets dropped by the netdevice and
// the number of packets requeued by the queuing discipline are reported.
//
// If the size of the DropTail queue of the netdevice were increased from 1
// to a large number (e.g. 1000), one would observe that the number of dropped
// packets goes to zero, but the latency grows in an uncontrolled manner. This
// is the so-called bufferbloat problem, and illustrates the importance of
// having a small device queue, so that the standing queues build in the traffic
// control layer where they can be managed by advanced queue discs rather than
// in the device layer.

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TrafficControlExample");

/**
 * Number of packets in TX queue trace.
 *
 * \param oldValue Old velue.
 * \param newValue New value.
 */
void
TcPacketsInQueueTrace(uint32_t oldValue, uint32_t newValue)
{
    std::cout << "TcPacketsInQueue " << oldValue << " to " << newValue << std::endl;
}

/**
 * Packets in the device queue trace.
 *
 * \param oldValue Old velue.
 * \param newValue New value.
 */
void
DevicePacketsInQueueTrace(uint32_t oldValue, uint32_t newValue)
{
    std::cout << "DevicePacketsInQueue " << oldValue << " to " << newValue << std::endl;
}

/**
 * TC Soujoun time trace.
 *
 * \param sojournTime The soujourn time.
 */
void
SojournTimeTrace(Time sojournTime)
{
    std::cout << "Sojourn time " << sojournTime.ToDouble(Time::MS) << "ms" << std::endl;
}

int
main(int argc, char* argv[])
{
    double simulationTime = 10; // simulation time in seconds
    std::string transportProt = "Udp"; //tcp or udp
    {
    std::string socketType;

    CommandLine cmd(__FILE__);
    cmd.AddValue("transportProt", "Transport protocol to use: Tcp, Udp", transportProt);
    cmd.Parse(argc, argv);

    if (transportProt == "Tcp") {
        socketType = "ns3::TcpSocketFactory";
    }
    else
    {
        socketType = "ns3::UdpSocketFactory";
    }

    NodeContainer nodes;
    nodes.Create(5);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("30Mbps")); //bandwidth change from 5, 15, 30
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms")); //delay
    pointToPoint.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("10p"));

    //node connectivity
    NetDeviceContainer devices01;
    devices01 = pointToPoint.Install(nodes.Get(0), nodes.Get(1));
    
    NetDeviceContainer devices12;
    devices12 = pointToPoint.Install(nodes.Get(1), nodes.Get(2));
    
    NetDeviceContainer devices02;
    devices02 = pointToPoint.Install(nodes.Get(0), nodes.Get(2));
    
    NetDeviceContainer devices13;
    devices13 = pointToPoint.Install(nodes.Get(1), nodes.Get(3));
    
    NetDeviceContainer devices34;
    devices34 = pointToPoint.Install(nodes.Get(3), nodes.Get(4));
    
      
    NetDeviceContainer devices24;
    devices24= pointToPoint.Install(nodes.Get(2), nodes.Get(4));

    InternetStackHelper stack;
    stack.Install(nodes);
    
    //Node interfaces and node address declaration
    Ipv4AddressHelper address01;
    address01.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces01 = address01.Assign(devices01);
    
    Ipv4AddressHelper address12;
    address12.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces12 = address12.Assign(devices12);
    
    Ipv4AddressHelper address02;
    address02.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces02 = address02.Assign(devices02);
    
    Ipv4AddressHelper address13;
    address13.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces13 = address13.Assign(devices13);
     
     Ipv4AddressHelper address34;
     address34.SetBase("10.1.5.0", "255.255.255.0");
     Ipv4InterfaceContainer interfaces34 = address34.Assign(devices34);
     
    Ipv4AddressHelper address24;
    address24.SetBase("10.1.6.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces24 = address24.Assign(devices24);

    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();


    // Flow
    uint16_t port = 7;
    Address localAddress(InetSocketAddress(Ipv4Address::GetAny(), port));
    PacketSinkHelper packetSinkHelper(socketType, localAddress);
    ApplicationContainer sinkApp = packetSinkHelper.Install(nodes.Get(4)); //4-destiation node

    sinkApp.Start(Seconds(0.0));
    sinkApp.Stop(Seconds(simulationTime + 0.1));
    uint32_t payloadSize = 1448;
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(payloadSize));

    OnOffHelper onoff(socketType, Ipv4Address::GetAny());
    onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    onoff.SetAttribute("PacketSize", UintegerValue(payloadSize));
    onoff.SetAttribute("DataRate", StringValue("50Mbps")); // dataraate
    ApplicationContainer apps;

    InetSocketAddress rmt(interfaces34.GetAddress(1), port); //34: interface - destination & 1: neighbour node address
    onoff.SetAttribute("Remote", AddressValue(rmt));
    onoff.SetAttribute("Tos", UintegerValue(0xb8));
    apps.Add(onoff.Install(nodes.Get(0))); //sender
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(simulationTime + 0.1));

    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    Simulator::Stop(Seconds(simulationTime + 5));
    Simulator::Run();

    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
    std::cout << std::endl << "*** Flow monitor statistics ***" << std::endl;
    std::cout << "  Tx Packets/Bytes:   " << stats[1].txPackets << " / " << stats[1].txBytes
              << std::endl;
   
    std::cout << "  Rx Packets/Bytes:   " << stats[1].rxPackets << " / " << stats[1].rxBytes
              << std::endl;
    
    std::cout << "  Throughput: "
              << stats[1].rxBytes * 8.0 /
                     (stats[1].timeLastRxPacket.GetSeconds() -
                      stats[1].timeFirstRxPacket.GetSeconds()) /
                     1000000
              << " Mbps" << std::endl;
    std::cout << "  Packets/Bytes Dropped:   " << stats[1].lostPackets
              << std::endl;
    
    std::cout << "  Mean delay:   " << stats[1].delaySum.GetSeconds() / stats[1].rxPackets
              << std::endl;
    std::cout << "  Mean jitter:   " << stats[1].jitterSum.GetSeconds() / (stats[1].rxPackets - 1)
              << std::endl;
    
    Simulator::Destroy();

    
    return 0;
}
}
