//@titlepage
// <center><h1>OBS Modules for OMNeT++/OMNEST</h1></center>
// <center>release 2013-01-14</center>
//
//
// OBS Modules implement all basic components required to simulate an OBS network. Functionalities are isolated over submodules in order to keep it as modular as possible.
//
// A complete OBS network architecture can be represented as follows:
//
// <center><img src=../img/OBSNet.jpg></center>
//
// The optical part of this network contains 2 kinds of nodes:
//
// <ul>
//    <li> <b>Edge node</b> (OBS_EdgeNode): Acts as an interface between optical and conventional (electronic) networks. It supports Ethernet, PPP and OBS networks. However, only one OBS interface can be defined now.
//OBS Interface (OBS_EdgeInterface) handles 2 submodules:
//       <ol>
//	    <li> An <b>assembler</b> (OBS_BurstAssembler) which receives 'electronic' messages and sends optical bursts. This process is divided in 3 steps:
//             <ul>
//				  <li> First of all, the message arrives in a <b>dispatcher</b> (OBS_PacketDispatcher) which decides the output gate for this message based on a group of rules.
//                <li> Next, there's the burst assembly step. One or more <b>packet burstifiers</b> (OBS_PacketBurstifier) assemble bursts with all received messages. This module is highly customizable with various methods of burst assembly (using a timer, maximum size, maximum number of messages). Also, you can set a padding and an overflow last packet condition.
//                <li> Finally, a <b>burst sender</b> (OBS_BurstSender), receives optical bursts and decides where and when to send them. It also implements OBS signalling using burst control packets (BCP).
//             </ul>
//             It also provides <b>file burstifiers</b> (OBS_FileBurstifier) which generate 'fake' bursts (with no content) using data contained in a text file and pass them to the sender. 
//	    <li> A <b>disassembler</b> (OBS_BurstDisassembler) which receives Optical Bursts and retrieves all electrical messages it contains.
//	</ol>
// <li> <b>Core node</b> (OBS_CoreNode): This is a 100% optical switch. Converts control info (BCP) into electronic packets to process it. Based on control Unit decisions,
// bursts will be switched transparently (with no OE/EO conversion) over the optical cross-connect (OXC). This module contains the following components:
//    <ol>
//       <li> <b>Input module</b> (OBS_CoreInput): Splits all fiber's channels and forwards all control channels to the control unit. Data channels will be connected to the OXC.
//       <li> <b>Output module</b> (OBS_CoreOutput): Reorganizes all optical channels in order to connect them to the output.
//       <li> <b>Optical cross-connect (OXC)</b> (OBS_OpticalCrossConnect): Forwards incoming bursts based on control unit orders. If an incoming burst has not an output gate, it will be discarded.
//       <li> A <b>control unit interface</b> (OBS_CoreControlUnit) which contains the control unit logic divided in subcomponents. It is intended to be implementation-independent. This implementation of the control unit has the following submodules:
//          <ul>
//             <li> <b>Optical/Electrical converter</b> (OBS_OEConverter)
//             <li> <b>Output horizon table</b> (OBS_CoreOutputHorizon) which stores the time where each output's data channel is free.
//             <li> <b>Routing table</b> (OBS_CoreRoutingTable) contains useful info for the control unit about where to send a received burst based on the fiber number, the colour (lambda) and the label it contains.
//             <li> <b>Control Unit</b> (OBS_CoreControlLogic) decides using incoming BCP info and a routing table where to redirect incoming bursts. This info will be sent to OXC using direct method calls.
//             <li> <b>Electrical/Optical converter</b> (OBS_EOConverter)
//          </ul>
//   </ol>
// </ul>
// Besides OBS networks components, this framework contains other modules which can be helpful on simulations:
// - A <b>random burst dropper</b> (OBS_DropBurst) which discards bursts randomly. This module deletes all messages associated to the deleted burst (BCPs and burst messages).
// - An <b>optical monitor</b> (OBS_OpticalMonitor) which collects information of all optical messages that pass through it. This is a sort of optical sniffer.
// 
// For complete OBS networks examples, see Examples/ directory. For each example, you'll find README file explaining how to configure it. Also, omnetpp.ini has comment lines explaining initialization parameters.
//
// For individual components test, see the test/ directory.
//
// Related documentation:
//
// <a href=../doxy/index.html target="_top">C++ Source documentation (Doxygen)</a>

package obsmodules;

@license(LGPL);
