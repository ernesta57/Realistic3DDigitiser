'''--------------------------------------------------------------------'''
'''  digi_steer_3d.py                                                  '''
'''  Same as digi_steer.py, except when --do3DDigi is passed swaps     '''
'''  the VXD (vertex barrel/endcap) digitizer for Realistic3DDigitiser '''
'''  without editing anything inside mucoll-benchmarks. Everything     '''
'''  else (calorimeters, IT, OT, overlay, etc.) is built exactly as    '''
'''  digiAlgList.py already does.                                      '''
'''--------------------------------------------------------------------'''
from GaudiKernel.Constants import INFO, WARNING

from k4FWCore.parseArgs import parser
parser.add_argument(
    "--do3DDigi",
    help="Use Realistic3DDigitiser (3D-sensor geometry) for VXD instead of whatever digiAlgList.py would otherwise build",
    action="store_true",
)

# Collect Arguments
from digi_components.digi_args import get_digi_args
args = get_digi_args()

# Set Up Services
from digi_components.digi_services import set_digi_services
[evtsvc, geoservice, id_service] = set_digi_services(args)

# Import the Algorithm List
from digiAlgList import makeDigiAlgList
algList = makeDigiAlgList(args)

if args.do3DDigi:
    from Configurables import Realistic3DDigitiser, TrackerHitTimeWindowFilter

    vxd_names_to_remove = {
        "VXDBarrelDigitiser", "VXDEndcapDigitiser",            # simple (DDPlanarDigi) branch
        "VertexBarrelDigitiser", "VertexEndcapDigitiser",      # realistic (MuonCVXDDigitiser) branch
        "VXDBarrelTimeFilter", "VXDEndcapTimeFilter",          # time-filter alongside the realistic branch
    }
    algList = [alg for alg in algList if alg.name() not in vxd_names_to_remove]

    def _vxd_3d_with_time_filter(name, input_coll, output_name, rel_name, raw_rel_name, passed_name, time_window):
        raw_name = f"{output_name}_Raw"
        digi_alg = Realistic3DDigitiser(
            name,
            InputHits=[input_coll],
            OutputCollectionName=[raw_name],
            RelationColName=[rel_name],
            RawHitsLinkColName=[raw_rel_name],
            SimHitLocCollectionName=[passed_name],
        )
        filter_alg = TrackerHitTimeWindowFilter(
            f"{name}TimeFilter",
            InputHits=[raw_name],
            OutputHits=[output_name],
            TimeWindowMin=time_window[0],
            TimeWindowMax=time_window[1],
        )
        return [digi_alg, filter_alg]

    vxd_input_barrel = "OverlayVertexBarrelCollection" if args.doOverlayFull else "VertexBarrelCollection"
    vxd_input_endcap = "OverlayVertexEndcapCollection" if args.doOverlayFull else "VertexEndcapCollection"

    algList += _vxd_3d_with_time_filter(
        "VertexBarrelDigitiser3D", vxd_input_barrel, "VXDBarrelHits",
        "VXDBarrelHitsRelations", "VXDBarrelRawHitsRelations", "VertexBarrelHits_Passed",
        (-0.09, 0.15),
    )
    algList += _vxd_3d_with_time_filter(
        "VertexEndcapDigitiser3D", vxd_input_endcap, "VXDEndcapHits",
        "VXDEndcapHitsRelations", "VXDEndcapRawHitsRelations", "VertexEndcapHits_Passed",
        (-0.09, 0.15),
    )

'''-------------------------------------------------------------------'''
'''    Run the Digitization Algorithms in the ApplicationMgr          '''
'''-------------------------------------------------------------------'''
# Declare Input and Output for the IOSvc
from k4FWCore import IOSvc, ApplicationMgr
svc = IOSvc(
    "IOSvc",
    Input = ["sim_output.edm4hep.root"],  # Input file from simulation
    Output = "digi_output.edm4hep.root"   # Output file for digitization
)

# Run the Application Manager
ApplicationMgr(
    TopAlg = algList,
    EvtSel = 'NONE',
    EvtMax   = 10,
    ExtSvc = [evtsvc, geoservice],
    OutputLevel=INFO
)
