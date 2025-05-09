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
            title="Upravit organizaci"
            show={modalVersion === "update-organisation"}
            submitText="Uložit změny"
            submitButtonColor="warning"
            onSubmit={updateOrganisationHandler}
            isLoading={isLoading}
            onHide={() => setModalVersion("")}
        >
            <p>Upravujete organizaci: <b>{editedOrganisation.name}</b></p>

            <input
                type="text"
                className="form-control mb-2"
                defaultValue={editedOrganisation.name}
                onChange={(e) => setEditedOrganisation({...editedOrganisation, name: e.target.value})}
            />

            <textarea
                className="form-control mb-2"
                rows={3}
                defaultValue={editedOrganisation.description}
                onChange={(e) => setEditedOrganisation({...editedOrganisation, description: e.target.value})}
            />

            {alerts.map((alert, index) => (
                <Alert key={index} variant="danger" className="mt-2" dismissible>
                    {alert}
                </Alert>
            ))}
        </DefaultModal>
    );
}

export default OrganisationUpdateModal;