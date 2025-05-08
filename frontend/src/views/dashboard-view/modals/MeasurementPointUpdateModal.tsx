import { useState } from "react";
import organisationRequests, { Organisation } from "../../../../API/requests/organisationRequests";
import DefaultModal from "../../../components/modals/DefaultModal";
import { useOrganisationContext } from "../../../customHooks/useOrganisationsContext";
import { Alert, Form } from "react-bootstrap";
import { DashboardModalVersion } from "../Dashboard";
import measurementPointsRequests, { MeasurementPoint, UpdateMeasurementPointDtoIn } from "../../../../API/requests/measurementPointsRequests";

interface Props {
    modalVersion: 'update-measurement-point',
    setModalVersion: React.Dispatch<React.SetStateAction<DashboardModalVersion>>,
    editedMeasurementPoint: MeasurementPoint,
    acknowladgeUpdatedMeasurementPoint: (mp: MeasurementPoint) => void,
}


// TODO
const MeasurementPointUpdateModal = (props: Props) => {
    const {
        modalVersion, setModalVersion,
        editedMeasurementPoint,
        acknowladgeUpdatedMeasurementPoint
    } = props;

    const [isLoading, setIsLoading] = useState(false);
    const [alerts, setAlerts] = useState<string[]>([]);

    const [updatedMeasurementPoint, setUpdatedMeasurementPoint] = useState<UpdateMeasurementPointDtoIn>({
        id: editedMeasurementPoint._id,
        name: editedMeasurementPoint.name,
        description: editedMeasurementPoint.description,
    })

    const updateMeasurementPointHandler = async () => {
        try {
            setIsLoading(true);
            const result = await measurementPointsRequests.updateMeasurementPoint(updatedMeasurementPoint);
            if (result) {
                acknowladgeUpdatedMeasurementPoint(result);
                setModalVersion("");
                return;
            }
            setAlerts(["Error updating organisation. Please try again."]);
        }
        catch (error) {
            console.error("Error updating organisation: ", error);
            setAlerts(["Error updating organisation. Please try again."]);
        }
        finally { setIsLoading(false); }
    }


    return (
        <DefaultModal
            title="Update Measurement Point"
            show={modalVersion === "update-measurement-point"}
            submitText="Update"
            submitButtonColor="warning"
            onSubmit={updateMeasurementPointHandler}
            isLoading={isLoading}
            onHide={() => setModalVersion("")}
        >
            <Form>
                <Form.Group className="mb-3" controlId="name">
                    <Form.Label>Name</Form.Label>
                    <Form.Control
                        type="text"
                        value={updatedMeasurementPoint.name}
                        placeholder="Enter name"
                        minLength={3}
                        maxLength={50}
                        onChange={(e) => setUpdatedMeasurementPoint({ ...updatedMeasurementPoint, name: e.target.value ?? "" })}
                    />
                </Form.Group>
                <Form.Group className="mb-3" controlId="description">
                    <Form.Label>Description</Form.Label>
                    <Form.Control
                        type="text"
                        value={updatedMeasurementPoint.description}
                        placeholder="Enter description (optional)"
                        maxLength={500}
                        onChange={(e) => setUpdatedMeasurementPoint({ ...updatedMeasurementPoint, description: e.target.value ?? "" })}
                    />
                </Form.Group>
            </Form>

            {alerts.map((alert, index) => (
                <Alert key={index} variant="danger" className="mt-2">
                    {alert}
                </Alert>
            ))}
        </DefaultModal>
    );
}

export default MeasurementPointUpdateModal;