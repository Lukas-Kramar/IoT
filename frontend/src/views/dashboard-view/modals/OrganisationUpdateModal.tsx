import { useState } from "react";
import organisationRequests, { Organisation } from "../../../../API/requests/organisationRequests";
import DefaultModal from "../../../components/modals/DefaultModal";
import { useOrganisationContext } from "../../../customHooks/useOrganisationsContext";
import { Alert } from "react-bootstrap";
import { DashboardModalVersion } from "../Dashboard";

interface Props {
    modalVersion: 'update-organisation',
    setModalVersion: React.Dispatch<React.SetStateAction<DashboardModalVersion>>,
    editedOrganisation: Organisation,
}


// TODO
const OrganisationUpdateModal = (props: Props) => {
    const {
        modalVersion, setModalVersion,
        editedOrganisation
    } = props;

    const { updateOrganisation, selectOrganisation } = useOrganisationContext();

    const [isLoading, setIsLoading] = useState(false);
    const [alerts, setAlerts] = useState<string[]>([]);

    const updateOrganisationHandler = async () => {
        try {
            setIsLoading(true);
            // const result = await organisationRequests.deleteOrganisation(editedOrganisation._id);
            // if (result) {
            //     selectOrganisation(null);
            //     deleteOrganisation(editedOrganisation._id);
            //     setModalVersion("");
            //     return;
            // }
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
            title="Update Organisation"
            show={modalVersion === "update-organisation"}
            submitText="Update Organisation"
            submitButtonColor="warning"
            onSubmit={updateOrganisationHandler}
            isLoading={isLoading}
            onHide={() => setModalVersion("")}
        >
            <p>TODO ......... </p>

            {alerts.map((alert, index) => (
                <Alert key={index} variant="danger" className="mt-2" dismissible>
                    {alert}
                </Alert>
            ))}
        </DefaultModal>
    );
}

export default OrganisationUpdateModal;